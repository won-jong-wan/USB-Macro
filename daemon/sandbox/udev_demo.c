#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <libudev.h>
#include <errno.h>
#include <stdint.h>

#define TARGET_DEV_PATH "/dev/team_own_stm32"
#define TARGET_DRIVER_NAME "team_own_stm32"

// 패킷 구조체
typedef struct {
    uint32_t magic;
    uint8_t  info;
    uint16_t cmd_len;
    char     command[249];
} __attribute__((packed)) datapacket;

int open_device(const char *device_path) {
    int fd = open(device_path, O_RDWR);
    if (fd < 0) return -1;
    // Non-blocking 설정
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return fd;
}

int main() {
    struct udev *udev = udev_new();
    if (!udev) return 1;

    struct udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", "usb_interface");
    udev_monitor_enable_receiving(mon);
    
    int udev_fd = udev_monitor_get_fd(mon);
    int dev_fd = -1;

    printf(">> [SYSTEM] 안전 모니터링 시작. USB를 꽂으세요.\n");

    while (1) {
        struct pollfd fds[2];
        int nfds = 1;

        fds[0].fd = udev_fd;
        fds[0].events = POLLIN;

        if (dev_fd >= 0) {
            fds[1].fd = dev_fd;
            // POLLIN: 데이터 수신, POLLHUP: 장치 끊김(Hang up), POLLERR: 에러
            fds[1].events = POLLIN | POLLHUP | POLLERR; 
            nfds = 2;
        }

        int ret = poll(fds, nfds, -1);
        if (ret < 0) break;

        // -----------------------------------------------------------------
        // [1] Udev 이벤트 처리 (장치 연결/해제 감지)
        // -----------------------------------------------------------------
        if (fds[0].revents & POLLIN) {
            struct udev_device *dev = udev_monitor_receive_device(mon);
            if (dev) {
                const char *action = udev_device_get_action(dev);
                const char *driver = udev_device_get_property_value(dev, "DRIVER"); 

                // driver나 action이 NULL일 경우를 대비해 안전하게 체크
                if (action && driver && strcmp(driver, TARGET_DRIVER_NAME) == 0) {
                    if (strcmp(action, "bind") == 0) {
                        printf(">> [EVENT] 드라이버 감지! (%s)\n", driver);
                        if (dev_fd < 0) {
                            usleep(100000); // 파일 생성 대기
                            dev_fd = open_device(TARGET_DEV_PATH);
                            if (dev_fd >= 0) printf("   -> [OPEN] 연결 성공!\n");
                        }
                    } 
                    else if (strcmp(action, "unbind") == 0) {
                        printf(">> [EVENT] 드라이버 해제 감지.\n");
                        if (dev_fd >= 0) {
                            close(dev_fd);
                            dev_fd = -1;
                            printf("   -> [CLOSE] 안전하게 닫음 (by Udev)\n");
                        }
                    }
                }
                udev_device_unref(dev);
            }
        }

        // -----------------------------------------------------------------
        // [2] 데이터 수신 (장치가 열려 있을 때만 진입)
        // ★ 중요: Udev 이벤트 처리로 dev_fd가 -1이 되었다면 여기는 실행되지 않아야 함
        // -----------------------------------------------------------------
        if (dev_fd >= 0 && nfds == 2) {
            
            // (A) 치명적인 에러나 끊김(HUP)이 발생했는지 먼저 확인
            if (fds[1].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                printf(">> [EMERGENCY] 장치 끊김 감지 (POLLHUP/ERR)!\n");
                close(dev_fd);
                dev_fd = -1;
            }
            // (B) 데이터가 들어왔을 때 (POLLIN)
            else if (fds[1].revents & POLLIN) {
                datapacket pkt;
                memset(&pkt, 0, sizeof(pkt)); // ★ 메모리 초기화 (쓰레기값 방지)

                ssize_t len = read(dev_fd, &pkt, sizeof(datapacket));
                
                if (len > 0) {
                    // 정상 데이터 수신
                    printf("[DATA] Len:%zd | Magic:0x%08X | Cmd:%.20s\n", 
                           len, pkt.magic, pkt.command);
                } 
                else {
                    // 읽기 에러 (장치가 뽑혔을 때 주로 발생)
                    // 에러가 났다면 즉시 닫아야 함
                    if (errno != EAGAIN) {
                        printf(">> [ERROR] 읽기 실패. 장치 닫음.\n");
                        close(dev_fd);
                        dev_fd = -1;
                    }
                }
            }
        }
    }

    udev_monitor_unref(mon);
    udev_unref(udev);
    return 0;
}