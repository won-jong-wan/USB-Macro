/*
 * msc.c
 *
 *  Created on: Dec 7, 2025
 *      Author: jonwo
 */
#include <string.h>
#include <stdbool.h>

#include "tusb.h"
#include "msc.h"

// --- 설정 ---
// STM32F411RE는 RAM이 128KB입니다.
// 넉넉하게 16KB(32섹터) 정도를 디스크로 써봅시다.
// 실제 디스크 역할을 할 메모리 배열

uint8_t msc_disk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE];

#define DEFAULT_FILE_CONTENT "MODE=TURBO\r\nSPEED=100"

void init_disk_data(void)
{
  // 1. 전체 초기화
  memset(msc_disk, 0, sizeof(msc_disk));

  // 2. [LBA 0] 부트 섹터 (ID 변경된 버전 유지)
  uint8_t const boot_sector[] = {
    0xEB, 0x3C, 0x90, 'M', 'S', 'D', 'O', 'S', '5', '.', '0',
    0x00, 0x02, 0x01, 0x01, 0x00, 0x02, 0x10, 0x00,
    (uint8_t)(DISK_BLOCK_NUM & 0xFF), (uint8_t)(DISK_BLOCK_NUM >> 8),
    0xF8, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x30, 0x12, 0x34, 0x56, 0x78,
    'S', 'T', 'M', '3', '2', 'F', '4', '1', '1', 'R', 'E',
    'F', 'A', 'T', '1', '2', ' ', ' ', ' '
  };
  memcpy(msc_disk[0], boot_sector, sizeof(boot_sector));
  msc_disk[0][510] = 0x55; msc_disk[0][511] = 0xAA;

  // 3. [LBA 1, 2] FAT 테이블
  // Cluster 2(첫 번째 데이터)를 파일의 끝(EOF)으로 표시: FF 0F
  uint8_t fat_data[] = { 0xF0, 0xFF, 0xFF, 0xFF, 0x0F };
  memcpy(msc_disk[1], fat_data, sizeof(fat_data));
  memcpy(msc_disk[2], fat_data, sizeof(fat_data));

  // 4. [LBA 3] 루트 디렉토리 설정
  uint8_t* root_dir = msc_disk[3];

  // (1) 볼륨 레이블 (Entry 0: 0~31 byte)
  uint8_t const vol_entry[] = {
	'S', 'T', 'M', '3', '2', 'F', '4', '1', '1', 'R', 'E',
    0x08, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0, 0,0, 0,0,0,0
  };
  memcpy(root_dir, vol_entry, 32);

  // (2) 파일 엔트리 "CONFIG.TXT" (Entry 1: 32~63 byte)
  uint8_t const file_entry[] = {
    'C', 'O', 'N', 'F', 'I', 'G', ' ', ' ', 'T', 'X', 'T', // 이름 (11자)
    0x20,                                                   // 속성 (Archive)
    0,0,0,0,0,0,0,0,0,0,                                    // 예약
    0,0,0,0,                                                // 시간/날짜
    0x02, 0x00,                                             // 시작 클러스터 (2번)
    (uint8_t)strlen(DEFAULT_FILE_CONTENT), 0, 0, 0          // 파일 크기
  };
  memcpy(root_dir + 32, file_entry, 32);

  // 5. [LBA 4] 실제 데이터 영역 (Cluster 2)에 내용 쓰기
  // 여기가 파일의 실제 내용이 들어가는 곳입니다.
  memcpy(msc_disk[4], DEFAULT_FILE_CONTENT, strlen(DEFAULT_FILE_CONTENT));
}

// LBA 4번(Cluster 2)이 CONFIG.TXT의 시작 위치임을 우리는 알고 있습니다.
// 복잡한 파일 시스템 파싱 없이, 그냥 배열 주소로 접근하면 됩니다.

void check_usb_file_command(void)
{
  // 1. 데이터 영역 포인터 가져오기
  char* file_content = (char*)msc_disk[4];

  // 2. 내용 검색 (strstr 함수 사용)
  // 사용자가 메모장을 열고 "MODE=TURBO"라고 저장했는지 확인
  if (strstr(file_content, "MODE=TURBO") != NULL)
  {
    // [터보 모드 동작]
    // 예: LED를 아주 빠르게 깜빡임
    // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    // HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  }
  else if (strstr(file_content, "MODE=ECO") != NULL)
  {
    // [에코 모드 동작]
    // 예: LED를 천천히 깜빡임
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    // HAL_Delay(1000);
  }
  else
  {
    // 기본 동작
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
  }
}

// ---------------------------------------------------------
// TinyUSB Callbacks
// ---------------------------------------------------------

// Inquiry
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
  (void) lun;
  memcpy(vendor_id,  "STM32   ", 8);
  memcpy(product_id, "RAM Disk        ", 16);
  memcpy(product_rev, "1.0 ", 4);
}

// Ready Check
bool tud_msc_test_unit_ready_cb(uint8_t lun) { (void) lun; return true; }

// Capacity
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
  (void) lun;
  *block_count = DISK_BLOCK_NUM;
  *block_size  = DISK_BLOCK_SIZE;
}

// READ (이제 배열에서 그냥 읽으면 됩니다!)
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
  (void) lun;
  uint8_t* ptr = (uint8_t*)buffer;

  for (uint32_t i = 0; i < bufsize / 512; i++) {
    uint32_t current_lba = lba + i;

    if (current_lba < DISK_BLOCK_NUM) {
      memcpy(ptr, msc_disk[current_lba] + offset, 512);
    } else {
      memset(ptr, 0, 512); // 범위 밖은 0 처리
    }
    ptr += 512;
  }
  return bufsize;
}

// WRITE (이제 배열에 쓰면 됩니다!)
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
  (void) lun;
  uint8_t* ptr = (uint8_t*)buffer;

  for (uint32_t i = 0; i < bufsize / 512; i++) {
    uint32_t current_lba = lba + i;

    if (current_lba < DISK_BLOCK_NUM) {
      memcpy(msc_disk[current_lba] + offset, ptr, 512);
    }
    ptr += 512;
  }
  return bufsize;
}

// SCSI
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
  (void) lun; (void) scsi_cmd; (void) buffer; (void) bufsize;
  return -1;
}
