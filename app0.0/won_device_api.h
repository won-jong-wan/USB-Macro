#ifndef WON_DEVICE_API_H
#define WON_DEVICE_API_H

#include <string>
#include <QDebug>
extern "C" {
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#pragma pack(push, 1)
struct datapacket {
    uint32_t magic;      // 보통 특정 값(0xDEADBEEF 등)으로 통신 확인용
    uint8_t info;
    uint16_t cmd_len;
    char command[249];
};
#pragma pack(pop)
}

datapacket createPacket(uint8_t version, uint8_t type, bool start, bool isEnd, const std::string& cmd);

void printPacket(const datapacket& packet);
class WonDeviceApi {
public:
    // 인스턴스 접근 (포인터 방식 유지)
    static WonDeviceApi* getInstance() {
        static WonDeviceApi instance;   // C++11 이후 자동 thread-safe
        return &instance;
    }

    // 복사 및 대입 금지
    WonDeviceApi(const WonDeviceApi&) = delete;
    WonDeviceApi& operator=(const WonDeviceApi&) = delete;

    int openDevice(const std::string& path = "/dev/team_own_stm32") {
        qDebug() << "try open device";

        if (fd != -1) {
            qDebug() << "opened";
            return fd; // 이미 열려있다면 기존 fd 반환
        }
        fd = open(path.c_str(), O_RDWR | O_NOCTTY);
        if (fd == -1) {
            qDebug() << "open failed errno =" << errno << strerror(errno);
        }


        qDebug() << "open" <<fd;
        return fd;
    }

    void closeDevice() {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }

    bool isOpen() const { return fd != -1; }

    ssize_t writePacket(const datapacket& packet) {
        qDebug() << "write!!!";
        if (fd == -1) return -1;
        return write(fd, &packet, sizeof(datapacket));
    }

    ssize_t readPacket(datapacket& packet) {
        if (fd == -1) return -1;
        return read(fd, &packet, sizeof(datapacket));
    }

private:
    WonDeviceApi() : fd(-1) {} // 생성자는 private
    ~WonDeviceApi() { closeDevice(); }
    int fd;
};

#endif
