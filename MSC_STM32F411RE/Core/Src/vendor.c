/*
 * vendor.c
 *
 *  Created on: Dec 16, 2025
 *      Author: jonwo
 */
#include <usb_class.h>
#include <string.h>
#include <stdio.h>  // sprintf용
#include <main.h>   // HAL_UART_Transmit용

#define TYPE 0x08
#define IS_END 0x04

#pragma pack(push, 1)
typedef struct {
	uint32_t magic;
	uint8_t  info;	/* 4bit version *//* 1bit type *//* 1bit isEnd *//* 2bits ZERO */
	uint16_t cmd_len;
	char     command[249];
} datapacket_t;
#pragma pack(pop)

static uint8_t g_accum_buf[512]; // 조각난 데이터를 합칠 공간
static uint32_t g_accum_cnt = 0; // 현재 모인 개수

static uint8_t msg_dma[512];

// Vendor 데이터 수신 콜백
void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint32_t bufsize)
{
	(void) itf;    // 사용 안 함
	(void) buffer; // [중요] 매개변수 무시! (믿지 않음)
	(void) bufsize;// [중요] 매개변수 무시! (믿지 않음)

	// 1. TinyUSB 내부에 진짜 쌓여있는 데이터 양을 확인
	uint32_t available = tud_vendor_available();

	// 2. 데이터가 진짜 있다면?
	if (available == 0) return; // check 1: is available?

	uint8_t my_buffer[64]; // 우리가 직접 마련한 그릇

	// 3. 수동으로 읽어오기 (여기서 FIFO를 비웁니다)
	// 읽어온 바이트 수를 리턴받음
	uint32_t count = tud_vendor_read(my_buffer, sizeof(my_buffer));

	// 읽어온 만큼만 처리
	if (count == 0) return; // check 2: data exist?

	// [디버깅] LED 토글 (데이터 진짜 읽음!)
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // LED BLINK

	if (g_accum_cnt + count > sizeof(datapacket_t)){
		g_accum_cnt = 0; // 에러! 너무 많이 왔으면 초기화
	}

	memcpy(&g_accum_buf[g_accum_cnt], my_buffer, count);  // Attach
	g_accum_cnt += count;

	if (g_accum_cnt < sizeof(datapacket_t)) return; // check 3: is complete?

	// 구조체 포인터 연결
	datapacket_t *pkt = (datapacket_t*)g_accum_buf;

	//////////////////////////
	// UART 디버깅 메시지 출력
	//////////////////////////
	int len = 0;

	// [방법 1] 내용을 한 방에 모아서 한 번만 보내기 (가장 추천)
	// 이렇게 하면 끊김 없이 깔끔하게 나옵니다.
	len += sprintf((char*)msg_dma + len, "\r\n--- Packet Received ---\r\n");
	len += sprintf((char*)msg_dma + len, "[RX] Magic: 0x%08lX\r\n", pkt->magic);
	len += sprintf((char*)msg_dma + len, "[RX] Info : 0x%02X\r\n", pkt->info);
	len += sprintf((char*)msg_dma + len, "[RX] Cmd  : %s\r\n", pkt->command);
	len += sprintf((char*)msg_dma + len, "-----------------------\r\n");

	// DMA 대신 일반 Transmit 사용 (Timeout: 100ms)
	// 이 함수는 다 보낼 때까지 여기서 기다립니다.
	HAL_UART_Transmit_DMA(&huart2, msg_dma, len);

	strcpy(pkt->command, "ECHO: Message Received");

	// 길이 정보 갱신
	pkt->cmd_len = strlen(pkt->command);
	pkt->info = 0x10|TYPE|IS_END; // TYPE이 애매함

	// (3) PC로 다시 보내기 (Echo)
	// 쓰기 버퍼 공간이 충분한지 확인
	if (tud_vendor_write_available() > sizeof(datapacket_t))
	{
		tud_vendor_write(g_accum_buf, sizeof(datapacket_t));
		tud_vendor_write_flush(); // 즉시 전송
	}

	// (4) 다음 패킷을 위해 카운터 초기화 [필수!]
	g_accum_cnt = 0;
}

//void vendor_task()
//{
//	static uint32_t start_ms = 0;
//
//	// 1초마다 실행
//	if (HAL_GetTick() - start_ms < 1000) return;
//	start_ms += 1000;
//
//    char *msg = "1234\n";
//    // 1초마다 "Alive"라는 메시지를 강제로 큐에 넣음
//
//    // 배열의 이름(hello)은 그 자체로 주소값이므로 그대로 넣어줍니다.
//    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)msg, 6);
//}
