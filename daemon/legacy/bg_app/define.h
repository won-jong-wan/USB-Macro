#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <stdint.h>
#include <sys/ioctl.h>
#define SET_VER(info, ver) ((info = info 0b11100000))

typedef struct {
  uint32_t magic;

  uint8_t info;

  uint16_t cmd_len;
  char command[249];
} __attribute__((packed)) datapacket;

int usb_open_device(const char *dev_path);
int usb_write_packet(int fd, const datapacket *packet);
int usb_read_packet(int fd, datapacket *packet);
int usb_control_reset(int fd);
void usb_close_device(int fd);

#define MY_USB_MAGIC 'U'
#define MY_USB_RESET _IO(MY_USB_MAGIC, 0x01)
#define MY_USB_SET_PACKET _IOW(MY_USB_MAGIC, 0x03, datapacket)

#endif
