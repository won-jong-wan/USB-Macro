/*
 * msc.h
 *
 *  Created on: Dec 7, 2025
 *      Author: jonwo
 */

#ifndef INC_USB_CLASS_H_
#define INC_USB_CLASS_H_

#include <main.h>
#include <tusb.h>

#define ROOT_DIR_LBA    3    // 루트 디렉토리 시작 위치
#define MAX_ROOT_SECTORS 1   // 루트 디렉토리 크기 (Entry 개수/16)
#define DATA_START_LBA  4   // 실제 데이터 시작 위치 (루트 끝난 다음)

#ifndef SCSI_CMD_MODE_SENSE_10
#define SCSI_CMD_MODE_SENSE_10    0x5A
#endif

#ifndef SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL
#define SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL 0x1E
#endif

#define PACK_INFO(ver, type, isEnd) \
    ((((ver) & 0xF) << 4) | (((type) & 0x1) << 3) | (((isEnd) & 0x1) << 2))

// 값을 꺼낼 때
#define GET_VER(info)   (((info) >> 4) & 0xF)
#define GET_TYPE(info)  (((info) >> 3) & 0x1)
#define GET_END(info)   (((info) >> 2) & 0x1)

// 3. 버퍼 설정 (32KB 단위 전송)
#define CHUNK_SIZE_KB 32
#define PACKET_COUNT  (CHUNK_SIZE_KB * 1024 / sizeof(datapacket)) // 128개
#define SD_BLOCKS     (CHUNK_SIZE_KB * 1024 / 512)                // 64 blocks

// 1바이트 정렬 (Packed) 필수!
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
    uint16_t high_cluster;// 클러스터 상위 2바이트 (FAT12에선 0)
    uint16_t mtime;       // 수정 시간
    uint16_t mdate;       // 수정 날짜
    uint16_t start_cluster; // [핵심] 데이터가 저장된 시작 위치
    uint32_t file_size;     // 파일 크기
} fat_dir_entry_t;
#pragma pack(pop)

typedef enum {
  SD_STATE_READY,   // 대기 상태 (명령 가능)
  SD_STATE_BUSY_TX, // 쓰기 중
  SD_STATE_BUSY_RX, // 읽기 중
  SD_STATE_ERROR    // 에러 발생
} SD_AsyncState;

// 실제 디스크 역할을 할 메모리 배열
extern UART_HandleTypeDef huart2;

void init_disk_data(void);
void check_usb_file_smart(void);

void hid_task(void);
//void vendor_task(void);

int SD_Write_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks);
int SD_Read_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks);
void SD_test(void);

void Process_SD_Write_Request(void);

#endif /* INC_USB_CLASS_H_ */
