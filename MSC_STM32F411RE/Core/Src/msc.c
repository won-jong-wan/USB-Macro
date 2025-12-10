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

#define DEFAULT_FILE_CONTENT "MODE=TURBO\r\n"

void init_disk_data(void)
{
  // 1. 전체 초기화
  memset(msc_disk, 0, sizeof(msc_disk));

  // --------------------------------------------------------
  // 2. [LBA 0] 부트 섹터 (Boot Sector)
  // --------------------------------------------------------
  uint8_t const boot_sector[] = {
    0xEB, 0x3C, 0x90,             // Jump Instruction
    'M', 'S', 'D', 'O', 'S', '5', '.', '0', // OEM Name
    0x00, 0x02,                   // Bytes per Sector (512)
    0x01,                         // Sectors per Cluster (1)
    0x01, 0x00,                   // Reserved Sectors (1 -> LBA 0)
    0x02,                         // Number of FATs (2)
    0x10, 0x00,                   // Root Dir Entries (16 -> 16*32 = 512 bytes = 1 sector)
    (uint8_t)(DISK_BLOCK_NUM & 0xFF), (uint8_t)(DISK_BLOCK_NUM >> 8), // Total Sectors (Small)
    0xF8,                         // Media Descriptor (0xF8: HDD/Generic) <--- [중요]
    0x01, 0x00,                   // Sectors per FAT (1)
    0x01, 0x00,                   // Sectors per Track
    0x01, 0x00,                   // Number of Heads
    0x00, 0x00, 0x00, 0x00,       // Hidden Sectors
    0x00, 0x00, 0x00, 0x00,       // Large Sectors
    0x80,                         // Drive Number
    0x00,                         // Reserved
    0x29,                         // Extended Boot Signature
    0x30, 0x12, 0x34, 0x56,       // Volume Serial Number
    'S', 'T', 'M', '3', '2', ' ', 'U', 'S', 'B', ' ', ' ', // Volume Label (11 bytes)
    'F', 'A', 'T', '1', '2', ' ', ' ', ' '  // FS Type
  };
  memcpy(msc_disk[0], boot_sector, sizeof(boot_sector));
  msc_disk[0][510] = 0x55; msc_disk[0][511] = 0xAA; // Boot Signature

  // --------------------------------------------------------
  // 3. [LBA 1, 2] FAT 테이블 (FAT1, FAT2)
  // --------------------------------------------------------
  // FAT12 구조: 12비트씩 패킹됨.
  // Entry 0 (Media): 0xFF8 (F8 + F)
  // Entry 1 (Rsrvd): 0xFFF (F + FF)
  // Entry 2 (File) : 0xFFF (EOF) -> FF + 0F
  //
  // Byte 0: Entry 0 (Low 8)  = 0xF8
  // Byte 1: Entry 1 (Low 4) << 4 | Entry 0 (High 4) = 0xF | 0xF0 = 0xFF
  // Byte 2: Entry 1 (High 8) = 0xFF
  // Byte 3: Entry 2 (Low 8)  = 0xFF
  // Byte 4: Entry 2 (High 4) = 0x0F
  // 결과: F8 FF FF FF 0F

  // [수정] 0xF0(Floppy)가 아니라 0xF8(HDD)로 시작해야 함!
  uint8_t fat_data[] = { 0xF8, 0xFF, 0xFF, 0xFF, 0x0F };
  memcpy(msc_disk[1], fat_data, sizeof(fat_data));
  memcpy(msc_disk[2], fat_data, sizeof(fat_data));

  // --------------------------------------------------------
  // 4. [LBA 3] 루트 디렉토리
  // --------------------------------------------------------
  uint8_t* root_dir = msc_disk[3];

  // (1) 볼륨 레이블 (Entry 0)
  uint8_t const vol_entry[] = {
    'S', 'T', 'M', '3', '2', ' ', 'U', 'S', 'B', ' ', ' ',
    0x08, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0, 0,0, 0,0,0,0
  };
  memcpy(root_dir, vol_entry, 32);

  // (2) 파일 엔트리 "CONFIG.TXT" (Entry 1)
  uint8_t const file_entry[] = {
    'C', 'O', 'N', 'F', 'I', 'G', ' ', ' ', 'T', 'X', 'T',
    0x20,                                   // 속성 (Archive)
    0,0,0,0,0,0,0,0,0,0,                    // 예약
    0x21, 0x54,                             // 시간 (대략 10:33:02) - 0이면 일부 OS 싫어함
    0x69, 0x54,                             // 날짜 (대략 2022-03-09)
    0x02, 0x00,                             // 시작 클러스터 (2번)
    (uint8_t)strlen(DEFAULT_FILE_CONTENT), 0, 0, 0  // 파일 크기
  };
  memcpy(root_dir + 32, file_entry, 32);

  // --------------------------------------------------------
  // 5. [LBA 4] 데이터 영역 (Cluster 2)
  // --------------------------------------------------------
  memcpy(msc_disk[4], DEFAULT_FILE_CONTENT, strlen(DEFAULT_FILE_CONTENT));
}

// LBA 4번(Cluster 2)이 CONFIG.TXT의 시작 위치임을 우리는 알고 있습니다.
// 복잡한 파일 시스템 파싱 없이, 그냥 배열 주소로 접근하면 됩니다.

void check_usb_file_command(void)
{
  // 디스크 전체 블록을 뒤집니다 (0, 1, 2번은 시스템 영역이니 3번부터)
  for (int i = 3; i < DISK_BLOCK_NUM; i++)
  {
    char* sector_data = (char*)msc_disk[i];

    // 해당 섹터에 "MODE=" 라는 글자가 있는지 확인
    if (strstr(sector_data, "MODE=LINUX") != NULL)
    {
      // [찾았다!]

      // 1. 반응 (LED)
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // LED 켜기

      // 2. (선택) 찾은 위치 디버깅용으로 출력하고 싶다면 Vendor interface로 쏠 수 있음
      // char msg[32];
      // sprintf(msg, "Found at LBA %d\n", i);
      // tud_vendor_write(msg, strlen(msg));
      // tud_vendor_write_flush();

      // 한 번 찾으면 루프 종료 (중복 실행 방지)
      return;
    }
    else if (strstr(sector_data, "MODE=TURBO") != NULL)
    {
       // 다른 모드 동작...
       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // LED 끄기
       return;
    }
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
