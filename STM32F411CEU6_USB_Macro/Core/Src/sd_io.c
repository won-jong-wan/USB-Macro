/*
 * sd_io.c
 *
 *  Created on: Dec 23, 2025
 *      Author: jonwo
 */
#include <main.h>
#include <buffer_event.h>
#include <tusb.h>

#include <string.h>
#include <stdio.h>

//#define FLAG_WAS_CHECKED

extern SD_HandleTypeDef hsd;
extern TIM_HandleTypeDef htim11; // 시간 측정용

uint32_t g_sd_block_nbr;

uint32_t g_msc_start_address;
uint32_t g_vendor_start_address;

volatile uint8_t g_sd_state;
volatile uint8_t g_sd_card_state;
uint8_t sd_buffer_need_flash = false;
uint32_t g_sd_buf_header = 0;

volatile task_q_t task_q;

//uint8_t g_msc2sd_buffer[4 * KB];
//uint8_t g_vendor2sd_buffer[BLOCK_SIZE];
uint8_t g_sd_buffer[BUFFER_SIZE]__attribute__((aligned(4))); // DMA 최적화

void SD_Init(void) {
	HAL_SD_CardInfoTypeDef pCardInfo;
	HAL_SD_GetCardInfo(&hsd, &pCardInfo);

	g_sd_block_nbr = pCardInfo.BlockNbr;

	g_vendor_start_address = g_sd_block_nbr / 2;
	g_ven_header = g_vendor_start_address;
	g_ven_info_addr = g_ven_header++;

	printf("[SD_Init] g_sd_block_nbr: %ld g_vendor_start_address: %ld\n\n",
			g_sd_block_nbr, g_vendor_start_address);

//	vendor_info_update(0);
}

void SD_Buffer_Reset(void) {
#ifdef TIME_CHECK
	__HAL_TIM_SET_COUNTER(&htim11, 0);
#endif // TIME_CHECK
	memset(g_sd_buffer, 0, BUFFER_SIZE);
#ifdef TIME_CHECK
	printf("SD reset: %ldus \n", __HAL_TIM_GET_COUNTER(&htim11));
#endif // TIME_CHECK
}

void SD_Test(void) {
	int8_t test_block_size = 1;

	SD_Buffer_Reset();

	// struct reset
	venpack_t test_pack;
	test_pack.magic = 0xBEEFCAFE;
	test_pack.info = 0x1C;
	const char *com = "hello everyone";
	test_pack.cmd_len = strlen(com);
	memcpy(test_pack.command, com, test_pack.cmd_len + 1);

	// write
	memcpy(g_sd_buffer, &test_pack, VENPACK_SIZE);
	printf("[Write] %s \n", test_pack.command);
	SD_Write_DMA_Async(g_vendor_start_address, g_sd_buffer, test_block_size);

	// buffer reset
	SD_Buffer_Reset();

	// read
	SD_Read_DMA_Async(g_vendor_start_address, g_sd_buffer, test_block_size);

	venpack_t *read_pack = (venpack_t*) g_sd_buffer;
	printf("[Read] %s \n", read_pack->command);
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) {
	g_sd_state = SD_STATE_READY; // 쓰기 완료 -> 대기 상태로 복귀
//	printf("[Tx]\n");
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) {
	g_sd_state = SD_STATE_READY; // 읽기 완료 -> 대기 상태로 복귀
//	printf("[Rx]\n");
}

void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd) {
	g_sd_state = SD_STATE_ERROR; // 에러 발생
	printf("[Err] Code: %ld, State: %d\n", hsd->ErrorCode, hsd->State);
}

int SD_Check_Card_State(void) { // SD 카드 상태를 확인하는 시간 약 10us
#ifdef TIME_CHECK
	__HAL_TIM_SET_COUNTER(&htim11, 0);
#endif // TIME_CHECK
	while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER) { // HAL_SD_GetCardState(&hsd)이 함수가 개느림
#ifdef ASYNC
		return g_sd_card_state;
#endif // ASYNC
//		tud_task();
	} // 유사 동기
#ifdef TIME_CHECK
	printf("SD card wait: %ldus \n", __HAL_TIM_GET_COUNTER(&htim11));
#endif // TIME_CHECK
	g_sd_card_state = SD_STATE_READY;
	return g_sd_card_state;
}

int SD_Check_State(void) {
#ifdef TIME_CHECK
	__HAL_TIM_SET_COUNTER(&htim11, 0);
#endif // TIME_CHECK
	while (g_sd_state != SD_STATE_READY) {
#ifdef ASYNC
		return g_sd_state;
#endif // ASYNC
//		tud_task();
	} // 유사 동기
#ifdef TIME_CHECK
	printf("SD DMA wait: %ldus \n", __HAL_TIM_GET_COUNTER(&htim11));
#endif // TIME_CHECK
	return g_sd_state;
}

int SD_Write_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks) {
#ifndef FLAG_WAS_CHECKED
	SD_Check_Card_State();
#endif // FLAG_WAS_CHECKED

	g_sd_state = SD_STATE_BUSY_TX;
	g_sd_card_state = SD_STATE_BUSY_TX;
	if (HAL_SD_WriteBlocks_DMA(&hsd, buf, lba, nblocks) != HAL_OK) {
		g_sd_state = SD_STATE_ERROR;
		return -3;
	}

#ifndef FLAG_WAS_CHECKED
	SD_Check_State();
#endif // FLAG_WAS_CHECKED

	return 0;
}

int SD_Read_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks) {
#ifndef FLAG_WAS_CHECKED
	SD_Check_Card_State();
#endif // FLAG_WAS_CHECKED

	g_sd_state = SD_STATE_BUSY_RX;
	g_sd_card_state = SD_STATE_BUSY_RX;
	if (HAL_SD_ReadBlocks_DMA(&hsd, buf, lba, nblocks) != HAL_OK) {
		g_sd_state = SD_STATE_ERROR;
		return -3;
	}

#ifndef FLAG_WAS_CHECKED
	SD_Check_State();
#endif // FLAG_WAS_CHECKED

	return 0; // 즉시 리턴
}
