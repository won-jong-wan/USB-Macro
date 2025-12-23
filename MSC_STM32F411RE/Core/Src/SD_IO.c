/*
 * sd_io.c
 *
 *  Created on: Dec 23, 2025
 *      Author: jonwo
 */

#include <main.h>
#include <buffer_event.h>

#include <string.h>
#include <stdio.h>

//#define FLAG_WAS_CHECKED

#define MAX_BLOCK 64 // 64 ~ 256이 최적
#define BLOCK_SIZE 512

#define BUFFER_SIZE MAX_BLOCK*BLOCK_SIZE // 64 * 512 = 32kb
                                         // 128 * 512 = 64kb // STM32F411RE MAX RAM = 128kb
extern SD_HandleTypeDef hsd;
extern UART_HandleTypeDef huart2;

uint32_t g_sd_block_nbr;

uint32_t g_msc_start_address;
uint32_t g_vendor_start_address;

volatile uint8_t g_sd_state;
volatile uint8_t g_sd_card_state;
uint8_t sd_buffer_need_flash = false;

volatile task_q_t task_q;

uint8_t g_msc2sd_buffer;
uint8_t g_vendor2sd_buffer;
uint8_t sd_buffer[BUFFER_SIZE]__attribute__((aligned(4))); // DMA 최적화

static uint32_t time = 0; // 속도는 결국 버퍼의 문제

void SD_Init(void){
	HAL_SD_CardInfoTypeDef pCardInfo;
	HAL_SD_GetCardInfo(&hsd, &pCardInfo);

	g_sd_block_nbr = pCardInfo.BlockNbr;

	g_vendor_start_address = g_sd_block_nbr/2;

	printf("[SD_Init] g_sd_block_nbr: %ld g_vendor_start_address: %ld\n", g_sd_block_nbr, g_vendor_start_address);
}

void SD_Buffer_Reset(void){
	memset(sd_buffer, 0, BUFFER_SIZE);
}

void SD_Test(void){
	SD_Buffer_Reset();

	venpack_t test_pack;
	test_pack.magic = 0xBEEFCAFE;
	test_pack.info = 0x1C;
	const char* com = "hello everyone";
	test_pack.cmd_len = strlen(com);
	memcpy(test_pack.command, com, test_pack.cmd_len );

	memcpy(sd_buffer, &test_pack, VENPACK_SIZE);
	printf("[Write] %s \n", test_pack.command);
	time = HAL_GetTick();
	SD_Write_DMA_Async(g_vendor_start_address, sd_buffer, 1);

	while(g_sd_state != SD_STATE_READY){} // 유사 동기
	printf("Write time: %ld \n", HAL_GetTick() - time);

	SD_Buffer_Reset();
	printf("[BefRead] %s\n", sd_buffer);
	time = HAL_GetTick();
	SD_Read_DMA_Async(g_vendor_start_address, sd_buffer, 1);

	while(g_sd_state != SD_STATE_READY){}

	venpack_t* read_pack = (venpack_t*)sd_buffer;
	printf("[Read] %s \n", read_pack->command);
	printf("Read time: %ld \n", HAL_GetTick() - time);
}

void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) {
	g_sd_state = SD_STATE_READY; // 쓰기 완료 -> 대기 상태로 복귀
	printf("[Tx]\n");
}

void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) {
	g_sd_state = SD_STATE_READY; // 읽기 완료 -> 대기 상태로 복귀
	printf("[Rx]\n");
}

void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd) {
	g_sd_state = SD_STATE_ERROR; // 에러 발생
	printf("[Err]\n");
}

int SD_Write_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks) {
#ifndef FLAG_WAS_CHECKED
  if (g_sd_state != SD_STATE_READY) return -1; // Busy
  while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER){} // 유사 동기
#endif //NON_CHECKED

	g_sd_state = SD_STATE_BUSY_TX;
	if (HAL_SD_WriteBlocks_DMA(&hsd, buf, lba, nblocks) != HAL_OK) {
		g_sd_state = SD_STATE_ERROR;
		return -3;
	}

	return 0;
}

int SD_Read_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks) {
#ifndef FLAG_WAS_CHECKED
  if (g_sd_state != SD_STATE_READY) return -1;
  while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER){} // 유사 동기
#endif // NON_CHECKED

	g_sd_state = SD_STATE_BUSY_RX;
	if (HAL_SD_ReadBlocks_DMA(&hsd, buf, lba, nblocks) != HAL_OK) {
		g_sd_state = SD_STATE_ERROR;
		return -3;
	}

	return 0; // 즉시 리턴
}
