#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/kfifo.h>
#include <linux/poll.h>
#include <linux/spinlock.h>
#include <linux/byteorder/generic.h>
#include "usb_struct.h"

#define USB_VENDOR_ID  0xCAFE
#define USB_PRODUCT_ID 0x4000

#define DRIVER_NAME    "team_own_stm32"
#define DEVICE_NAME    "team_own_stm32"
#define MAX_PKT_SIZE   256    // FS Bulk max 64바이트 (필요하면 lsusb로 확인해서 맞추기)
#define RX_FIFO_SIZE 8192

// ---------- USB 매칭 테이블 ----------
static struct usb_device_id usb_device_table[] = {
  { USB_DEVICE_AND_INTERFACE_INFO(USB_VENDOR_ID, USB_PRODUCT_ID, 0xFF, 0x00, 0x00) },
  {}
};
MODULE_DEVICE_TABLE(usb, usb_device_table);

// ---------- 디바이스 상태 구조체 ----------
struct stm32_usb_dev {
    struct usb_device            *udev;
    struct usb_interface         *interface;

    
    struct usb_endpoint_descriptor *bulk_in;
    struct usb_endpoint_descriptor *bulk_out;

    struct cdev                  cdev;
    struct mutex                 io_mutex;
      unsigned char           *bulk_in_buffer;
    size_t                  bulk_in_size;
    struct urb              *bulk_in_urb;

    /* 수신 데이터 관리 */
    struct kfifo            rx_fifo;
    spinlock_t              rx_lock;
    atomic_t                disconnected;
    wait_queue_head_t       read_wq;

    atomic_t open_count;
    wait_queue_head_t close_wq;

};
static void stm32_bulk_in_callback(struct urb *urb);
static ssize_t stm32_usb_read(struct file *file, char __user *buf,
                          size_t count, loff_t *ppos);
static int try_pop_frame(struct stm32_usb_dev *dev,
                         u8 *out, size_t out_sz,
                         size_t *frame_len);
                         
static dev_t stm32_dev_t;
static struct class *stm32_class;
static struct stm32_usb_dev *g_dev;   // 예제 단순화를 위해 전역 1개만
// static atomic_t dev_minor_alloc = ATOMIC_INIT(0); /* 단일이면 없어도 됨 */

// ---------- write 완료 콜백 ----------
static void stm32_usb_write_complete(struct urb *urb)
{
    unsigned char *buf = urb->transfer_buffer;

    if (urb->status)
        printk(KERN_ERR DRIVER_NAME ": write complete status=%d\n", urb->status);

    kfree(buf);
    usb_free_urb(urb);
}

// ---------- file_operations::write ----------
static ssize_t stm32_usb_write(struct file *file,
                               const char __user *user_buf,
                               size_t count, loff_t *ppos)
{
    struct stm32_usb_dev *dev = file->private_data;
    struct urb *urb;
    unsigned char *buf;
    int retval;

    if (!dev || !dev->udev)
        return -ENODEV;

    if (!count)
        return 0;

    if (count > MAX_PKT_SIZE)
        count = MAX_PKT_SIZE;

    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb)
        return -ENOMEM;

    buf = kmalloc(count, GFP_KERNEL);
    if (!buf) {
        usb_free_urb(urb);
        return -ENOMEM;
    }

    if (copy_from_user(buf, user_buf, count)) {
        kfree(buf);
        usb_free_urb(urb);
        return -EFAULT;
    }

    mutex_lock(&dev->io_mutex);

    usb_fill_bulk_urb(urb,
                      dev->udev,
                      usb_sndbulkpipe(dev->udev,
                                      dev->bulk_out->bEndpointAddress),
                      buf,
                      count,
                      stm32_usb_write_complete,
                      dev);

    retval = usb_submit_urb(urb, GFP_KERNEL);

    mutex_unlock(&dev->io_mutex);

    if (retval) {
        printk(KERN_ERR DRIVER_NAME ": usb_submit_urb(write) failed: %d\n", retval);
        kfree(buf);
        usb_free_urb(urb);
        return retval;
    }

    // 비동기니까 보낸 길이만큼 성공했다고 리턴
    return count;
}
static ssize_t stm32_usb_read(struct file *file, char __user *buf,
                          size_t count, loff_t *ppos)
{
   struct stm32_usb_dev *dev = file->private_data;
    u8 frame[DP_HDR_SIZE + DP_MAX_PAYLOAD];
    size_t flen;
    int ret;

    if (!dev)
        return -ENODEV;

    if (atomic_read(&dev->disconnected))
        return -ENODEV;

    for (;;) {
        ret = try_pop_frame(dev, frame, sizeof(frame), &flen);
        if (ret == 1)
            break;          // 프레임 1개 완성
        if (ret < 0)
            return ret;     // -EMSGSIZE 등

        if (file->f_flags & O_NONBLOCK)
            return -EAGAIN;

        ret = wait_event_interruptible(dev->read_wq,
                atomic_read(&dev->disconnected) ||
                kfifo_len(&dev->rx_fifo) >= DP_HDR_SIZE);
        if (ret)
            return -ERESTARTSYS;

        if (atomic_read(&dev->disconnected))
            return -ENODEV;
    }

    if (count < flen)
        return -EMSGSIZE;

    if (copy_to_user(buf, frame, flen))
        return -EFAULT;

    return flen;
}

static __poll_t stm32_usb_poll(struct file *file, poll_table *wait)
{
    struct stm32_usb_dev *dev = file->private_data;

    poll_wait(file, &dev->read_wq, wait);

    if (atomic_read(&dev->disconnected))
        return POLLERR | POLLHUP;

    if (kfifo_len(&dev->rx_fifo) >= DP_HDR_SIZE)
        return POLLIN | POLLRDNORM;

    return 0;
}

// ---------- file_operations::open / release ----------
static int stm32_usb_open(struct inode *inode, struct file *file)
{
 struct stm32_usb_dev *dev = g_dev;
    if (!dev)
        return -ENODEV;

    if (atomic_read(&dev->disconnected))
        return -ENODEV;

    atomic_inc(&dev->open_count);
    file->private_data = dev;
    return 0;
}

static int stm32_usb_release(struct inode *inode, struct file *file)
{
    struct stm32_usb_dev *dev = file->private_data;
    if (dev) {
        if (atomic_dec_and_test(&dev->open_count))
            wake_up_interruptible(&dev->close_wq);
    }
    return 0;
}

// ---------- file_operations ----------
static const struct file_operations stm32_usb_fops = {
    .owner   = THIS_MODULE,
    .open    = stm32_usb_open,
    .release = stm32_usb_release,
    .read    = stm32_usb_read,
    .write   = stm32_usb_write,
    .poll    = stm32_usb_poll,
};

// ---------- USB probe ----------
static int stm32_usb_probe(struct usb_interface *interface,
                           const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(interface);
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    struct stm32_usb_dev *dev;
    struct device *d;
    int i, retval;
    printk(KERN_INFO DRIVER_NAME ": Device connected(VID=0x%04x PID=0x%04x)\n", id->idVendor, id->idProduct);

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->udev      = usb_get_dev(udev);
    dev->interface = interface;
    mutex_init(&dev->io_mutex);

    init_waitqueue_head(&dev->read_wq);
    init_waitqueue_head(&dev->close_wq);
    atomic_set(&dev->open_count, 0);
    atomic_set(&dev->disconnected, 0);
    spin_lock_init(&dev->rx_lock);

    iface_desc = interface->cur_altsetting;
    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;
        if (usb_endpoint_is_bulk_in(endpoint))
            dev->bulk_in = endpoint;
        else if (usb_endpoint_is_bulk_out(endpoint))
            dev->bulk_out = endpoint;
    }

    if (!dev->bulk_in || !dev->bulk_out) {
        retval = -ENODEV;
        goto err_putdev;
    }

    dev->bulk_in_size   = usb_endpoint_maxp(dev->bulk_in);
    dev->bulk_in_buffer = kzalloc(dev->bulk_in_size, GFP_KERNEL);
    if (!dev->bulk_in_buffer) {
        retval = -ENOMEM;
        goto err_putdev;
    }

    dev->bulk_in_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!dev->bulk_in_urb) {
        retval = -ENOMEM;
        goto err_inbuf;
    }

    retval = kfifo_alloc(&dev->rx_fifo, RX_FIFO_SIZE, GFP_KERNEL);
    if (retval)
        goto err_urb;

    usb_fill_bulk_urb(dev->bulk_in_urb,
                      dev->udev,
                      usb_rcvbulkpipe(dev->udev, dev->bulk_in->bEndpointAddress),
                      dev->bulk_in_buffer,
                      dev->bulk_in_size,
                      stm32_bulk_in_callback,
                      dev);

    retval = usb_submit_urb(dev->bulk_in_urb, GFP_KERNEL);
    if (retval)
        goto err_kfifo;

    cdev_init(&dev->cdev, &stm32_usb_fops);
    dev->cdev.owner = THIS_MODULE;

    retval = cdev_add(&dev->cdev, stm32_dev_t, 1);
    if (retval)
        goto err_killurb;

    d = device_create(stm32_class, &interface->dev, stm32_dev_t, NULL, DEVICE_NAME);
    if (IS_ERR(d)) {
        retval = PTR_ERR(d);
        goto err_cdev;
    }

    usb_set_intfdata(interface, dev);
    g_dev = dev;
    return 0;

err_cdev:
    cdev_del(&dev->cdev);
err_killurb:
    usb_kill_urb(dev->bulk_in_urb);
err_kfifo:
    kfifo_free(&dev->rx_fifo);
err_urb:
    usb_free_urb(dev->bulk_in_urb);
    dev->bulk_in_urb = NULL;
err_inbuf:
    kfree(dev->bulk_in_buffer);
    dev->bulk_in_buffer = NULL;
err_putdev:
    usb_put_dev(dev->udev);
    kfree(dev);
    return retval;
}


// ---------- USB disconnect ----------
static void stm32_usb_disconnect(struct usb_interface *interface)
{
    struct stm32_usb_dev *dev = usb_get_intfdata(interface);

    printk(KERN_INFO DRIVER_NAME ": Device disconnect\n");

    if (!dev)
        return;

    usb_set_intfdata(interface, NULL);

    /* 더 이상 IO 불가 표시 + read 깨우기 */
    atomic_set(&dev->disconnected, 1);
    wake_up_interruptible(&dev->read_wq);

    /* URB 중단(콜백 더 이상 실행 X) */
    if (dev->bulk_in_urb) {
        usb_kill_urb(dev->bulk_in_urb);
        usb_free_urb(dev->bulk_in_urb);
        dev->bulk_in_urb = NULL;
    }

    /* 열린 fd가 닫히길 기다림: interruptible보단 timeout 추천 */
    wait_event_timeout(dev->close_wq,
                       atomic_read(&dev->open_count) == 0,
                       msecs_to_jiffies(2000));

    /* char device 정리 */
    device_destroy(stm32_class, stm32_dev_t);
    cdev_del(&dev->cdev);

    /* RX 자원 해제 */
    kfree(dev->bulk_in_buffer);
    dev->bulk_in_buffer = NULL;
    kfifo_free(&dev->rx_fifo);

    /* usb/dev 정리 */
    usb_put_dev(dev->udev);

    g_dev = NULL;
    kfree(dev);
}


// ---------- usb_driver ----------
static struct usb_driver stm32_usb_driver = {
    .name       = DRIVER_NAME,
    .id_table   = usb_device_table,
    .probe      = stm32_usb_probe,
    .disconnect = stm32_usb_disconnect,
};

static void stm32_bulk_in_callback(struct urb *urb)
{
    struct stm32_usb_dev *dev = urb->context;
    unsigned long flags;
    unsigned int in;
    int rc;

    if (!dev)
        return;

    /* disconnect 이후엔 아무 것도 하지 않음 */
    if (atomic_read(&dev->disconnected))
        return;

    /* 에러 status면 재제출 여부만 결정 */
    if (urb->status) {
        /* disconnect/kill에서 흔히 나오는 값들은 조용히 종료 */
        if (urb->status == -ENOENT ||
            urb->status == -ECONNRESET ||
            urb->status == -ESHUTDOWN)
            return;

        pr_err(DRIVER_NAME ": bulk in urb status=%d\n", urb->status);
        /* 아래에서 재제출은 시도해볼 수 있음(상황에 따라) */
        goto resubmit;
    }

    /* 정상 수신 */
    if (urb->actual_length > 0) {
	pr_info(DRIVER_NAME ": RX %u bytes\n", urb->actual_length); // URB가 들어오는지 확인 
	// print_hex_dump(KERN_INFO,
	// 		DRIVER_NAME ": RX DATA: ",
	// 		DUMP_PREFIX_OFFSET,
	// 		16, 1,
	// 		urb->transfer_buffer, 
	// 		urb->actual_length, false);
        spin_lock_irqsave(&dev->rx_lock, flags);
        in = kfifo_in(&dev->rx_fifo, urb->transfer_buffer, urb->actual_length);
        spin_unlock_irqrestore(&dev->rx_lock, flags);

        if (in != urb->actual_length) {
            /* overflow: 스트림 리셋(네 정책 OK) */
            spin_lock_irqsave(&dev->rx_lock, flags);
            kfifo_reset(&dev->rx_fifo);
            spin_unlock_irqrestore(&dev->rx_lock, flags);

            pr_warn(DRIVER_NAME ": rx fifo overflow (drop/reset)\n");
        }

        wake_up_interruptible(&dev->read_wq);
    }

resubmit:
    if (atomic_read(&dev->disconnected))
        return;

    rc = usb_submit_urb(urb, GFP_ATOMIC);
    if (rc) {
        /* -ENODEV/-ESHUTDOWN면 사실상 이미 끊기는 중 */
        pr_err(DRIVER_NAME ": resubmit failed rc=%d (status=%d)\n", rc, urb->status);
        return;
    }
}
static int try_pop_frame(struct stm32_usb_dev *dev, u8 *out, size_t out_sz, size_t *frame_len)
{
    unsigned long flags;
    struct dp_hdr_wire wh;   /* wire header: 7 bytes packed */
    u16 len;
    size_t need;
    u8 drop;
    unsigned int copied;

    spin_lock_irqsave(&dev->rx_lock, flags); // 락 잡기 인터럽트/URB 보호 

    if (kfifo_len(&dev->rx_fifo) < DP_HDR_SIZE)
        goto no_frame;

    /* 헤더(7바이트)만 먼저 들여다보기 */
    copied = kfifo_out_peek(&dev->rx_fifo, &wh, DP_HDR_SIZE);
    if (copied != DP_HDR_SIZE)
        goto no_frame;

    /* magic 체크 */
    if (le32_to_cpu(wh.magic) != DP_MAGIC) {
        /* resync: 1바이트 버리기 */
        copied = kfifo_out(&dev->rx_fifo, &drop, 1);
        if (copied != 1)
            goto no_frame;
        goto no_frame;
    }

    /* cmd_len(bytes) payload 길이 검증 */
    len = le16_to_cpu(wh.cmd_len);
    if (len > DP_MAX_PAYLOAD) {
        copied = kfifo_out(&dev->rx_fifo, &drop, 1);
        if (copied != 1)
            goto no_frame;
        goto no_frame;
    }

    need = DP_HDR_SIZE + len; // 프레임 전체 크기 계산
			      //
    /*프레임이 전부 모였는지 확인  */ 
    if (kfifo_len(&dev->rx_fifo) < need)
        goto no_frame; 
    /*출력 버퍼 크기 확인 */
    if (out_sz < need) {
        spin_unlock_irqrestore(&dev->rx_lock, flags);
        return -EMSGSIZE;
    }

    /* 프레임 1개 pop */
    copied = kfifo_out(&dev->rx_fifo, out, need);
    if (copied != need)
        goto no_frame;

    *frame_len = need;

    spin_unlock_irqrestore(&dev->rx_lock, flags);
    return 1;

no_frame:
    spin_unlock_irqrestore(&dev->rx_lock, flags);
    return 0;
}

// ---------- 모듈 init/exit ----------
static int __init stm32_usb_init(void)
{
     int ret;

    printk(KERN_INFO DRIVER_NAME ": init\n");

    ret = alloc_chrdev_region(&stm32_dev_t, 0, 1, DRIVER_NAME);
    if (ret < 0)
        return ret;

    stm32_class = class_create(THIS_MODULE,DRIVER_NAME);
    if (IS_ERR(stm32_class)) {
        ret = PTR_ERR(stm32_class);
        stm32_class = NULL;
        unregister_chrdev_region(stm32_dev_t, 1);
        return ret;
    }

      ret = usb_register(&stm32_usb_driver);   // usb_driver 등록 
    if (ret) {
        class_destroy(stm32_class);
        stm32_class = NULL;
        unregister_chrdev_region(stm32_dev_t, 1);
        return ret;
    }

    return 0;
}

static void __exit stm32_usb_exit(void)
{
    printk(KERN_INFO DRIVER_NAME ": exit\n");

    usb_deregister(&stm32_usb_driver);

    if (stm32_class) {
        class_destroy(stm32_class);
        stm32_class = NULL;
    }

    unregister_chrdev_region(stm32_dev_t, 1);
}



module_init(stm32_usb_init);
module_exit(stm32_usb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team OWN");
MODULE_DESCRIPTION("Simple STM32 USB Bulk Driver");
