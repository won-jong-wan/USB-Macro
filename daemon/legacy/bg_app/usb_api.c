#include "define.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

// 1. 장치 열기
int usb_open_device(const char *dev_path) {
  int fd = open(dev_path, O_RDWR);
  if (fd < 0)
    perror("Device Open Failed");
  return fd;
}

// 2. 데이터 쓰기 (Write API 사용)
int usb_write_packet(int fd, const datapacket *packet) {
  ssize_t ret = write(fd, packet, sizeof(datapacket));
  if (ret < 0) {
    perror("Write Failed");
    return -1;
  }
  return (int)ret;
}

// 3. 데이터 읽기 (Read API 사용)
int usb_read_packet(int fd, datapacket *packet) {
  ssize_t ret = read(fd, packet, sizeof(datapacket));
  if (ret < 0) {
    perror("Read Failed");
    return -1;
  }
  return (int)ret;
}

// 4. 특수 제어 (IOCTL API 사용)
int usb_control_reset(int fd) { return ioctl(fd, MY_USB_RESET); }

int usb_set_config(int fd, datapacket *config) {
  return ioctl(fd, MY_USB_SET_PACKET, config);
}

// 5. 장치 닫기
void usb_close_device(int fd) { close(fd); }
