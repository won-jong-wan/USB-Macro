#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/wait.h>

#define USB_VENDOR_ID  0xDEAD
#define USB_PRODUCT_ID 0xBEEF

#define DRIVER_NAME    "team_own_stm32"
#define DEVICE_NAME    "team_own_stm32"
#define MAX_PKT_SIZE   64    // FS Bulk max 64바이트 (필요하면 lsusb로 확인해서 맞추기)

// ---------- USB 매칭 테이블 ----------
static struct usb_device_id usb_device_table[] = {
    { USB_DEVICE(USB_VENDOR_ID, USB_PRODUCT_ID) },
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
    size_t                  data_len;
    bool                    data_available;
    wait_queue_head_t       read_wq;

};
static void stm32_bulk_in_callback(struct urb *urb);
static ssize_t stm32_usb_read(struct file *file, char __user *buf,
                          size_t count, loff_t *ppos);
static dev_t stm32_dev_t;
static struct class *stm32_class;
static struct stm32_usb_dev *g_dev;   // 예제 단순화를 위해 전역 1개만

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
    int retval;

    if (!dev)
        return -ENODEV;

    mutex_lock(&dev->io_mutex);

    /* 데이터 없으면 대기 (블로킹 모드인 경우) */
    while (!dev->data_available) {
        mutex_unlock(&dev->io_mutex);

        if (file->f_flags & O_NONBLOCK)
            return -EAGAIN;

        /* 데이터 들어올 때까지 sleep */
        if (wait_event_interruptible(dev->read_wq, dev->data_available))
            return -ERESTARTSYS;  // 시그널로 깨어난 경우

        mutex_lock(&dev->io_mutex);
    }

    /* 유저가 요구한 크기보다 수신 데이터가 작으면 그만큼만 */
    if (count > dev->data_len)
        count = dev->data_len;

    /* 커널 버퍼 → 유저 버퍼로 복사 */
    if (copy_to_user(buf, dev->bulk_in_buffer, count)) {
        retval = -EFAULT;
        goto out;
    }

    /* 데이터 소비 완료 처리 */
    dev->data_available = false;
    dev->data_len = 0;

    /* 다음 데이터를 위해 다시 URB submit */
    usb_fill_bulk_urb(dev->bulk_in_urb,
                      dev->udev,
                      usb_rcvbulkpipe(dev->udev,
                                      dev->bulk_in->bEndpointAddress),
                      dev->bulk_in_buffer,
                      dev->bulk_in_size,
                      stm32_bulk_in_callback,
                      dev);

    retval = usb_submit_urb(dev->bulk_in_urb, GFP_KERNEL);
    if (retval) {
        pr_err("stm32: failed to resubmit bulk in urb: %d\n", retval);
        goto out;
    }

    retval = count;

out:
    mutex_unlock(&dev->io_mutex);
    return retval;
}



// ---------- file_operations::open / release ----------
static int stm32_usb_open(struct inode *inode, struct file *file)
{
    if (!g_dev)
        return -ENODEV;

    file->private_data = g_dev;
    return 0;
}

static int stm32_usb_release(struct inode *inode, struct file *file)
{
    return 0;
}

// ---------- file_operations ----------
static const struct file_operations stm32_usb_fops = {
    .owner   = THIS_MODULE,
    .open    = stm32_usb_open,
    .release = stm32_usb_release,
    .read    = stm32_usb_read,
    .write   = stm32_usb_write,
};

// ---------- USB probe ----------
static int stm32_usb_probe(struct usb_interface *interface,
                           const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(interface);
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    struct stm32_usb_dev *dev;
    int i, retval;

    printk(KERN_INFO DRIVER_NAME ": Device connected (VID=0x%04x PID=0x%04x)\n",
           id->idVendor, id->idProduct);

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->udev      = usb_get_dev(udev);
    dev->interface = interface;
    mutex_init(&dev->io_mutex);

    iface_desc = interface->cur_altsetting;
    for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
        endpoint = &iface_desc->endpoint[i].desc;

        if (usb_endpoint_is_bulk_in(endpoint)) {
            dev->bulk_in = endpoint;
            printk(KERN_INFO DRIVER_NAME ": found bulk IN ep 0x%02x\n",
                   endpoint->bEndpointAddress);
        } else if (usb_endpoint_is_bulk_out(endpoint)) {
            dev->bulk_out = endpoint;
            printk(KERN_INFO DRIVER_NAME ": found bulk OUT ep 0x%02x\n",
                   endpoint->bEndpointAddress);
        }
    }

    if (!dev->bulk_in || !dev->bulk_out) {
        printk(KERN_ERR DRIVER_NAME ": bulk IN/OUT endpoint not found\n");
        retval = -ENODEV;
        goto error;
    }
dev->bulk_in_size   = usb_endpoint_maxp(dev->bulk_in);
dev->bulk_in_buffer = kzalloc(dev->bulk_in_size, GFP_KERNEL);
if (!dev->bulk_in_buffer) {
    retval = -ENOMEM;
    goto error;
}

dev->bulk_in_urb = usb_alloc_urb(0, GFP_KERNEL);
if (!dev->bulk_in_urb) {
    retval = -ENOMEM;
    goto error;
}

/* wait queue & mutex 초기화 (한 번만 하면 됨) */
init_waitqueue_head(&dev->read_wq);
dev->data_available = false;
dev->data_len = 0;

/* IN URB 세팅 */
usb_fill_bulk_urb(dev->bulk_in_urb,
                  dev->udev,
                  usb_rcvbulkpipe(dev->udev,
                                  dev->bulk_in->bEndpointAddress),
                  dev->bulk_in_buffer,
                  dev->bulk_in_size,
                  stm32_bulk_in_callback,   // 너가 구현할 콜백
                  dev);

retval = usb_submit_urb(dev->bulk_in_urb, GFP_KERNEL);
if (retval) {
    printk(KERN_ERR DRIVER_NAME ": failed to submit bulk in URB: %d\n", retval);
    goto error;
}
    // char device 등록 (이 예제는 첫 probe 때만 한다고 가정)
    retval = alloc_chrdev_region(&stm32_dev_t, 0, 1, DRIVER_NAME);
    if (retval < 0) {
        printk(KERN_ERR DRIVER_NAME ": alloc_chrdev_region failed\n");
        goto error;
    }

    cdev_init(&dev->cdev, &stm32_usb_fops);
    dev->cdev.owner = THIS_MODULE;

    retval = cdev_add(&dev->cdev, stm32_dev_t, 1);
    if (retval) {
        printk(KERN_ERR DRIVER_NAME ": cdev_add failed\n");
        unregister_chrdev_region(stm32_dev_t, 1);
        goto error;
    }

    stm32_class = class_create(DRIVER_NAME);
    if (IS_ERR(stm32_class)) {
        printk(KERN_ERR DRIVER_NAME ": class_create failed\n");
        cdev_del(&dev->cdev);
        unregister_chrdev_region(stm32_dev_t, 1);
        retval = PTR_ERR(stm32_class);
        goto error;
    }

    device_create(stm32_class, &interface->dev, stm32_dev_t, NULL, DEVICE_NAME);

    usb_set_intfdata(interface, dev);
    g_dev = dev;

    printk(KERN_INFO DRIVER_NAME ": /dev/%s created\n", DEVICE_NAME);
    return 0;

error:
    if (dev->bulk_in_urb) {
        usb_kill_urb(dev->bulk_in_urb);
        usb_free_urb(dev->bulk_in_urb);
    }
    kfree(dev->bulk_in_buffer);

    if (dev->udev)
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

    if (dev->bulk_in_urb) {
    usb_kill_urb(dev->bulk_in_urb);
    usb_free_urb(dev->bulk_in_urb);
    }
    kfree(dev->bulk_in_buffer);
    
    device_destroy(stm32_class, stm32_dev_t);
    class_destroy(stm32_class);
    cdev_del(&dev->cdev);
    unregister_chrdev_region(stm32_dev_t, 1);

    usb_set_intfdata(interface, NULL);
    usb_put_dev(dev->udev);
    kfree(dev);
    g_dev = NULL;
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
    int status = urb->status;

    if (status) {
        /* -ENOENT, -ECONNRESET, -ESHUTDOWN 등은 disconnect 시 나오는 것 */
        if (status != -ENOENT && status != -ECONNRESET && status != -ESHUTDOWN)
            pr_err("stm32: bulk in error, status %d\n", status);
        return;
    }

    if (urb->actual_length > 0) {
        /* 데이터 들어옴 → dev 내부 버퍼에 이미 들어간 상태 */
        dev->data_len = urb->actual_length;
        dev->data_available = true;

        /* read()에서 wait중인 유저 프로세스 깨우기 */
        wake_up_interruptible(&dev->read_wq);
    }
    /* 지금은 단순화를 위해 read()에서 다시 submit. */
}


// ---------- 모듈 init/exit ----------
static int __init stm32_usb_init(void)
{
    printk(KERN_INFO DRIVER_NAME ": init\n");
    return usb_register(&stm32_usb_driver);
}

static void __exit stm32_usb_exit(void)
{
    printk(KERN_INFO DRIVER_NAME ": exit\n");
    usb_deregister(&stm32_usb_driver);
}



module_init(stm32_usb_init);
module_exit(stm32_usb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team OWN");
MODULE_DESCRIPTION("Simple STM32 USB Bulk Driver");