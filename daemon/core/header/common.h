#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define MAGIC_NUMBER 0xDEADBEEF

// 비트 필드 매크로
#define GET_VERSION(info) (((info) >> 4) & 0x0F)
#define GET_DELAY(info)   (((info) >> 3) & 0x01)
#define GET_START(info)   (((info) >> 2) & 0x01)
#define GET_ISEND(info)   (((info) >> 1) & 0x01)
#define GET_TYPE(info)    ((info) & 0x01)

// 패킷 구조체 (네트워크/USB 통신용 Packed 구조체)
typedef struct {
    uint32_t magic;
    uint8_t info;
    uint16_t cmd_len;
    char command[249];
} __attribute__((packed)) datapacket;

#endif
