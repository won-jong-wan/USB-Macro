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

#define DISK_BLOCK_NUM   128
#define DISK_BLOCK_SIZE  512

#define ROOT_DIR_LBA    3    // 루트 디렉토리 시작 위치
#define MAX_ROOT_SECTORS 1   // 루트 디렉토리 크기 (Entry 개수/16)
#define DATA_START_LBA  4   // 실제 데이터 시작 위치 (루트 끝난 다음)

#ifndef SCSI_CMD_MODE_SENSE_10
#define SCSI_CMD_MODE_SENSE_10    0x5A
#endif

#ifndef SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL
#define SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL 0x1E
#endif

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

// 실제 디스크 역할을 할 메모리 배열
extern uint8_t msc_disk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE];
extern UART_HandleTypeDef huart2;

void init_disk_data(void);
void check_usb_file_smart(void);

void hid_task(void);
//void vendor_task(void);

#endif /* INC_USB_CLASS_H_ */
