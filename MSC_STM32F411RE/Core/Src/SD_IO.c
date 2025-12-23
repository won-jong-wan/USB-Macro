/*
 * SD_IO.c
 *
 *  Created on: Dec 23, 2025
 *      Author: kccistc
 */
#include "main.h"
#include <stdio.h>
#include <string.h>

#include "usb_class.h"

volatile SD_AsyncState g_sd_state = SD_STATE_READY;

volatile uint8_t g_sd_tx_done = 0;
volatile uint8_t g_sd_rx_done = 0;

// 1. 구조체 정의
typedef struct {
  uint32_t magic;      // 4 bytes

  // [Info Field: 1 byte]
  // 7-4: Version(4), 3: Type(1), 2: IsEnd(1), 1-0: Zero(2)
  uint8_t info;

  uint16_t cmd_len;    // 2 bytes
  char command[249];   // 249 bytes
} __attribute__((packed)) datapacket; // Total: 256 Bytes

// 구조체 크기 검증 (필수)
_Static_assert(sizeof(datapacket) == 256, "Packet size is not 256 bytes!");

extern SD_HandleTypeDef hsd;
extern UART_HandleTypeDef huart2;

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) {
  g_sd_state = SD_STATE_READY; // 쓰기 완료 -> 대기 상태로 복귀
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) {
  g_sd_state = SD_STATE_READY; // 읽기 완료 -> 대기 상태로 복귀
}

void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd) {
  g_sd_state = SD_STATE_ERROR; // 에러 발생
}

//// printf용
//int _write(int file, char *ptr, int len) {
//  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 100);
//  return len;
//}

// 비동기 쓰기 시작 (기다리지 않음!)
int SD_Write_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks) {
  // 1. 카드가 이전 작업을 하고 있는지 확인 (필수)
  if (g_sd_state != SD_STATE_READY) return -1; // Busy
  while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER){} // Card Busy

  // 2. 상태 변경 후 DMA 시작
  g_sd_state = SD_STATE_BUSY_TX;
  if (HAL_SD_WriteBlocks_DMA(&hsd, buf, lba, nblocks) != HAL_OK) {
    g_sd_state = SD_STATE_ERROR;
    return -3;
  }

  return 0; // 즉시 리턴 (성공적으로 시작됨)
}

// 비동기 읽기 시작
int SD_Read_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks) {
  if (g_sd_state != SD_STATE_READY) return -1;
  while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER){};

  g_sd_state = SD_STATE_BUSY_RX;
  if (HAL_SD_ReadBlocks_DMA(&hsd, buf, lba, nblocks) != HAL_OK) {
    g_sd_state = SD_STATE_ERROR;
    return -3;
  }

  return 0; // 즉시 리턴
}
#ifdef TEST

#define CHUNK_SIZE_KB 32
#define STRUCT_PER_CHUNK (CHUNK_SIZE_KB * 1024 / sizeof(BigLog)) // 128개
#define BLOCKS_PER_CHUNK (CHUNK_SIZE_KB * 1024 / 512)            // 64 blocks
#define TEST_ITERATIONS 32

uint8_t w_buf[CHUNK_SIZE_KB * 1024] __attribute__((aligned(4)));
uint8_t r_buf[CHUNK_SIZE_KB * 1024] __attribute__((aligned(4)));

void SD_test(void){
	printf("\r\n=== Custom Packet (256B) SD Test ===\r\n");

	  // 4. 데이터 생성 (가상의 명령 패킷 만들기)
	  datapacket *pPack = (datapacket *)w_buf;

	  printf("Generating %d packets...\r\n", PACKET_COUNT);

	  for (int i = 0; i < PACKET_COUNT; i++) {
	      pPack[i].magic = 0xCAFEBABE; // 매직 넘버

	      // 비트 필드 설정 예시
	      // Version: 5, Type: 1, IsEnd: (마지막 패킷만 1, 나머지 0)
	      uint8_t ver = 5;
	      uint8_t type = 1;
	      uint8_t isEnd = (i == PACKET_COUNT - 1) ? 1 : 0;

	      pPack[i].info = PACK_INFO(ver, type, isEnd);

	      // 명령어 문자열 채우기
	      char temp_cmd[64];
	      sprintf(temp_cmd, "CMD_MOVE_X_%d", i * 10);

	      pPack[i].cmd_len = strlen(temp_cmd);
	      strncpy(pPack[i].command, temp_cmd, 249);
	  }

	  // 5. SD 카드에 쓰기 (LBA: 10000)
	  uint32_t target_lba = 10000;

	  printf("Writing to SD...\r\n");
	  if (SD_Write_DMA_Async(target_lba, w_buf, SD_BLOCKS) != 0) {
	      printf("Write Failed!\r\n");
	      Error_Handler();
	  }

	  // DMA 및 카드 저장 대기
	  while (g_sd_state != SD_STATE_READY) {}
	  while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER) {}
	  printf("Write Complete.\r\n");

	  // 6. SD 카드에서 읽기
	  printf("Reading back...\r\n");
	  memset(r_buf, 0, sizeof(r_buf)); // 버퍼 클리어

	  if (SD_Read_DMA_Async(target_lba, r_buf, SD_BLOCKS) != 0) {
	      printf("Read Failed!\r\n");
	      Error_Handler();
	  }

	  while (g_sd_state != SD_STATE_READY) {}
	  while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER) {}
	  printf("Read Complete.\r\n");

	  // 7. 데이터 검증 및 디코딩
	  datapacket *pRead = (datapacket *)r_buf;
	  int err_cnt = 0;

	  printf("\r\n--- Decoding First 3 Packets ---\r\n");
	  for (int i = 0; i < 3; i++) {
	      printf("[%d] Magic: 0x%lX | Ver: %d, Type: %d, End: %d | Cmd: %s\r\n",
	             i,
	             pRead[i].magic,
	             GET_VER(pRead[i].info),
	             GET_TYPE(pRead[i].info),
	             GET_END(pRead[i].info),
	             pRead[i].command);
	  }

	  // 전체 검수
	  for (int i = 0; i < PACKET_COUNT; i++) {
	      if (pRead[i].magic != 0xCAFEBABE) err_cnt++;
	      // 마지막 패킷의 isEnd 비트가 1인지 확인
	      if (i == PACKET_COUNT - 1) {
	          if (GET_END(pRead[i].info) != 1) {
	              printf("Last packet END flag missing!\r\n");
	              err_cnt++;
	          }
	      }
	  }

	  if (err_cnt == 0) {
	      printf("\r\n[SUCCESS] All packets verified perfectly!\r\n");
	  } else {
	      printf("\r\n[FAIL] %d errors found.\r\n", err_cnt);
	  }
}
#endif

