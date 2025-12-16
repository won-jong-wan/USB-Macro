/*
 * msc.c
 *
 *  Created on: Dec 7, 2025
 *      Author: jonwo
 */
#include <string.h>
#include <stdbool.h>
#include <usb_class.h>

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
	0x10, 0x00,  // Root entries
	(uint8_t)(DISK_BLOCK_NUM & 0xFF), (uint8_t)(DISK_BLOCK_NUM >> 8), // <-- 여기가 매크로인지 확인!
	0xF8,
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

void check_usb_file_smart(void)
{
    // 1. 루트 디렉토리 영역 탐색 (LBA 3 ~ 18)
    for (int lba = ROOT_DIR_LBA; lba < ROOT_DIR_LBA + MAX_ROOT_SECTORS; lba++)
    {
        uint8_t* sector = msc_disk[lba]; // 해당 섹터 포인터

        // 한 섹터(512B) 안에 디렉토리 엔트리(32B)가 16개 들어있음
        for (int i = 0; i < 512; i += 32)
        {
            fat_dir_entry_t* entry = (fat_dir_entry_t*) &sector[i];

            // 2. 파일 이름 비교
            // FAT 파일 시스템은 이름(8) + 확장자(3) 형태로 저장됨. (공백으로 채워짐)
            // "CONFIG.TXT" -> "CONFIG  TXT"

            // 첫 글자가 0x00이면 더 이상 파일 없음 (탐색 종료)
            if (entry->name[0] == 0x00) return;
            // 첫 글자가 0xE5이면 삭제된 파일 (건너뜀)
            if (entry->name[0] == 0xE5) continue;

            // 이름 "CONFIG  " 와 확장자 "TXT" 확인
            if (memcmp(entry->name, "CONFIG  ", 8) == 0 &&
                memcmp(entry->ext,  "TXT", 3) == 0)
            {
                // [찾았다!]

                // 3. 데이터 위치 계산
                // 클러스터 번호를 LBA로 변환
                // 공식: LBA = Data_Start + (Cluster - 2) * SectorsPerCluster
                // (우리는 SectorsPerCluster = 1로 설정했음)
                uint16_t cluster = entry->start_cluster;
                uint32_t target_lba = DATA_START_LBA + (cluster - 2);

                // 범위 체크 (안전장치)
                if (target_lba >= DISK_BLOCK_NUM) return;

                // 4. 내용 읽기
                char* file_content = (char*) msc_disk[target_lba];

                // 내용 확인 및 동작
                if (strstr(file_content, "MODE=LINUX") != NULL)
                {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // LED ON
                }
                else if (strstr(file_content, "MODE=WINDOW") != NULL)
                {
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // LED OFF
                }

                // 파일을 찾았으니 더 이상 탐색 불필요
                return;
            }
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

int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
  void const* response = NULL;
  int32_t resplen = 0;

  // 명령어 종류에 따라 처리
  switch ( scsi_cmd[0] )
  {
    // [중요] 리눅스가 장치 잠금/해제 시도할 때 OK 해줘야 함
    case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
      // 그냥 성공(0) 했다고 거짓말하면 됨
      return 0;

    // [중요] 리눅스는 MODE_SENSE_6 (0x1A)를 자주 씀
    case SCSI_CMD_MODE_SENSE_6:
    {
      // "쓰기 금지(Write Protect) 아님" 이라고 알려주는 헤더
      static uint8_t const mode_sense_data[4] = { 0x03, 0x00, 0x00, 0x00 };
      response = mode_sense_data;
      resplen  = 4;
      break;
    }

    // 윈도우가 쓰는 MODE_SENSE_10 (0x5A)
    case SCSI_CMD_MODE_SENSE_10:
    {
      static uint8_t const mode_sense_data[8] = { 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      response = mode_sense_data;
      resplen  = 8;
      break;
    }

    // 그 외 모르는 명령어는 거부 (-1)
    default:
      return -1;
  }

  // 응답 데이터 복사 (버퍼 오버플로우 방지)
  if ( response && resplen > 0 )
  {
    if ( resplen > bufsize ) resplen = bufsize;
    memcpy(buffer, response, resplen);
  }

  return resplen;
}
