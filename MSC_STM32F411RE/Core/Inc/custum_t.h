/*
 * vendor_struct.h
 *
 *  Created on: Dec 23, 2025
 *      Author: jonwo
 */

#ifndef INC_CUSTUM_T_H_
#define INC_CUSTUM_T_H_

// ------------- MSC
//#define MSCPACK_SIZE 256

#pragma pack(push, 1)
typedef struct {
	uint8_t name[8];      // 파일명 (8글자)
	uint8_t ext[3];       // 확장자 (3글자)
	uint8_t attr;         // 속성
	uint8_t reserved;
	uint8_t ctime_ms;     // 생성 시간 (밀리초)
	uint16_t ctime;       // 생성 시간
	uint16_t cdate;       // 생성 날짜
	uint16_t adate;       // 접근 날짜
	uint16_t high_cluster;       // 클러스터 상위 2바이트 (FAT12에선 0)
	uint16_t mtime;       // 수정 시간
	uint16_t mdate;       // 수정 날짜
	uint16_t start_cluster; // [핵심] 데이터가 저장된 시작 위치
	uint32_t file_size;     // 파일 크기
} fat_dir_entry_t;
#pragma pack(pop)

// -------------- Vendor
#define VENPACK_SIZE 256

#pragma pack(push, 1)
typedef struct {
	uint32_t magic;
	uint8_t info; /* 4bit version *//* 1bit type *//* 1bit isEnd *//* 2bits ZERO */
	uint16_t cmd_len;
	char command[249];
} venpack_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	uint32_t f_magic;
	uint32_t total_len;
	// 기타
	uint32_t checksum;
	uint32_t b_magic;
} sd_info_t;
#pragma pack(pop)


_Static_assert(sizeof(venpack_t) == 256, "Packet size is not 256 bytes!");

#define PACK_INFO(ver, type, isEnd) \
    ((((ver) & 0xF) << 4) | (((type) & 0x1) << 3) | (((isEnd) & 0x1) << 2))

// 값을 꺼낼 때
#define GET_VER(info)   (((info) >> 4) & 0xF)
#define GET_TYPE(info)  (((info) >> 3) & 0x1)
#define GET_END(info)   (((info) >> 2) & 0x1)

// ------------- SD_State

typedef enum {
	SD_STATE_READY,   // 대기 상태 (명령 가능)
	SD_STATE_BUSY_TX, // 쓰기 중
	SD_STATE_BUSY_RX, // 읽기 중
	SD_STATE_ERROR    // 에러 발생
} sd_state_t;

#endif /* INC_CUSTUM_T_H_ */
