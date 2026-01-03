/*
 * buffer_and_flag.h
 *
 *  Created on: Dec 23, 2025
 *      Author: jonwo
 */

#ifndef INC_BUFFER_EVENT_H_
#define INC_BUFFER_EVENT_H_

// -- include
#include <custum_t.h>
#include <task_q.h>

// -- define
//#define ASYNC
#define TIME_CHECK

#define KB 1024 // 1024 byte
#define MB KB*KB
#define GB KB*KB*KB

#define MAX_BLOCK 1 // 64 ~ 256이 최적
#define BLOCK_SIZE 512

#define BUFFER_SIZE MAX_BLOCK*BLOCK_SIZE // 64 * 512 = 32kb
                                         // 128 * 512 = 64kb // STM32F411RE MAX RAM = 128kb

enum {
	USB_MODE_CDC = 0,       // 긴급/디버그 모드 (CDC Only)
	USB_MODE_MSC_VENDOR,    // 펌웨어 업데이트 등 (MSC + Vendor)
	USB_MODE_HID_MSC,        // 평상시 사용 (HID + MSC)
};

// -- flag
extern volatile uint8_t g_usb_mode;
extern volatile bool usb_ready;

extern volatile uint8_t g_sd_state;
extern volatile uint8_t g_sd_card_state;

// -- val
extern uint32_t g_sd_block_nbr;
extern uint32_t g_sd_buf_header;

extern uint32_t g_msc_start_address;
extern uint32_t g_vendor_start_address;
extern uint32_t g_ven_header;
extern uint32_t g_ven_info_addr;

// -- buffer
extern volatile task_q_t task_q;

extern char msg_dma[512];
extern uint8_t g_msc2sd_buffer[4*KB];
extern uint8_t g_vendor2sd_buffer[BLOCK_SIZE];
extern uint8_t g_sd_buffer[BUFFER_SIZE]__attribute__((aligned(4)));

// -- func
extern void SD_Init(void);
extern void SD_Test(void);
extern int SD_Check_State(void);
extern int SD_Check_Card_State(void);
extern int SD_Write_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks);
extern int SD_Read_DMA_Async(uint32_t lba, uint8_t *buf, uint32_t nblocks);

extern void vendor_info_update(int len);
extern void ven_send(void);

#endif /* INC_BUFFER_EVENT_H_ */
