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
} datapacket;
#pragma pack(pop)

// --- SD Card Buffering Settings ---
#define CHUNK_SIZE_KB 32
#define PKT_SIZE      256  // sizeof(datapacket)
#define PKTS_PER_BUF  (CHUNK_SIZE_KB * 1024 / PKT_SIZE) // 128개

// 더블 버퍼 (32KB x 2)
// aligned(4)는 DMA 필수
uint8_t g_sd_buf[2][PKTS_PER_BUF * PKT_SIZE] __attribute__((aligned(4)));

// 버퍼 관리 변수
volatile uint8_t  g_cur_buf_idx = 0;   // USB가 현재 채우고 있는 버퍼 (0 or 1)
volatile uint16_t g_cur_pkt_cnt = 0;   // 현재 버퍼에 쌓인 패킷 개수

volatile int8_t   g_write_req_idx = -1; // SD 저장 요청된 버퍼 (-1: 없음, 0 or 1)
volatile uint16_t g_write_pkt_cnt = 0;  // 저장 요청된 패킷 개수 (isEnd 때문에 가변적)

static uint8_t g_accum_buf[512]; // 조각난 데이터를 합칠 공간
static uint32_t g_accum_cnt = 0; // 현재 모인 개수

extern volatile SD_AsyncState g_sd_state;
extern SD_HandleTypeDef hsd;

uint8_t msg_dma[512];

// 메인 루프에서 계속 호출해줘야 함
void Process_SD_Write_Request(void) {
    // 1. 요청이 없으면 리턴
    if (g_write_req_idx == -1) return;

    // 2. SD 카드가 바쁘면 리턴 (다음 턴에 처리)
    if (g_sd_state != SD_STATE_READY) return;
    if (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER) return;

    // 3. 저장할 블록 수 계산
    // 중요: SD카드는 512바이트(1블록) 단위로만 저장 가능.
    // 구조체(256B)가 홀수 개(예: 1개, 3개)면 0.5블록이 되므로 올림 처리해야 함.

    // 바이트 크기 = 패킷 수 * 256
    uint32_t total_bytes = g_write_pkt_cnt * PKT_SIZE;

    // 블록 수 = (바이트 + 511) / 512  -> 올림 나눗셈
    uint32_t n_blocks = (total_bytes + 511) / 512;

    // 홀수 개일 경우 뒷부분 쓰레기 값 방지를 위해 0으로 채우기 (선택사항)
    // (메모리가 256 단위로 정렬되어 있으니 다음 256바이트를 0으로 밀어버림)
    if ((g_write_pkt_cnt % 2) != 0) {
        uint8_t *buf_base = g_sd_buf[g_write_req_idx];
        memset(&buf_base[total_bytes], 0, 256);
    }

    // 4. DMA 쓰기 시작
    // LBA(주소) 관리는 전역변수로 따로 해야 함
    static uint32_t current_lba = 10000;

    if (SD_Write_DMA_Async(current_lba, g_sd_buf[g_write_req_idx], n_blocks) == 0) {
        // 성공적으로 시작됨
        current_lba += n_blocks; // 주소 증가

        // 요청 처리 완료 표시 -> -1
        // (주의: DMA 완료 콜백이 아니라 '시작' 시점에 플래그를 내리면
        //  아주 빠른 속도에서 꼬일 수 있으나, 현재 구조상 안전)
        g_write_req_idx = -1;

        // 디버깅용
         printf("Saved %d packets to LBA %lu\r\n", g_write_pkt_cnt, current_lba);
    }
}

#ifndef ECHO

void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint32_t bufsize)
{
    (void) itf; (void) buffer; (void) bufsize;

    if (tud_vendor_available() == 0) return;

    uint8_t my_buffer[64];
    uint32_t count = tud_vendor_read(my_buffer, sizeof(my_buffer));
    if (count == 0) return;

    // 1. 패킷 조립 (기존 로직)
    if (g_accum_cnt + count > PKT_SIZE) g_accum_cnt = 0; // Overflow 방지
    memcpy(&g_accum_buf[g_accum_cnt], my_buffer, count);
    g_accum_cnt += count;

    if (g_accum_cnt < PKT_SIZE) return; // 아직 256바이트 안 됨

    // 2. 패킷 완성! (256바이트)
    datapacket *pkt = (datapacket*)g_accum_buf;

    int len = 0;

    len += sprintf((char*)msg_dma + len, "\r\n--- Packet Received ---\r\n");
	len += sprintf((char*)msg_dma + len, "[RX] Magic: 0x%08lX\r\n", pkt->magic);
	len += sprintf((char*)msg_dma + len, "[RX] Info : 0x%02X\r\n", pkt->info);
	len += sprintf((char*)msg_dma + len, "[RX] Cmd  : %s\r\n", pkt->command);
	len += sprintf((char*)msg_dma + len, "-----------------------\r\n");

	HAL_UART_Transmit_DMA(&huart2, msg_dma, len);

    // --- [NEW] SD 더블 버퍼링 로직 시작 ---

    // (A) 현재 채우고 있는 SD 버퍼로 패킷 복사
    uint8_t *dest_addr = &g_sd_buf[g_cur_buf_idx][g_cur_pkt_cnt * PKT_SIZE];
    memcpy(dest_addr, pkt, PKT_SIZE);
    g_cur_pkt_cnt++;

    // (B) 트리거 조건 확인
    // 1. 버퍼가 꽉 찼거나 (128개)
    // 2. isEnd 플래그가 1이거나 (매크로 사용 가정)
    uint8_t is_end = GET_END(pkt->info);
    uint8_t is_full = (g_cur_pkt_cnt >= PKTS_PER_BUF);

    if (is_full || is_end) {
        // 만약 이전에 요청한 저장이 아직도 안 끝났다면? (Overflow)
        if (g_write_req_idx != -1) {
            // 에러 처리: LED를 켠다거나, Error Count 증가
            // 여기서는 일단 무시하고 덮어씁니다 (혹은 기다릴 수도 없음, 인터럽트라)
        }

        // 저장 요청 (Main Loop에게 신호)
        g_write_req_idx = g_cur_buf_idx; // "이 버퍼 가져가서 구워라"
        g_write_pkt_cnt = g_cur_pkt_cnt; // "몇 개 들어있는지도 알려줌"

        // 버퍼 교체 (Swap)
        g_cur_buf_idx = (g_cur_buf_idx == 0) ? 1 : 0;
        g_cur_pkt_cnt = 0; // 새 버퍼 카운트 리셋
    }
    // --- [NEW] SD 더블 버퍼링 로직 끝 ---


    // 3. PC로 Echo 응답 (기존 로직 유지)
    // 필요하다면 유지, SD 속도 최적화를 위해선 제거하는 게 좋음
    pkt->info = 0x10; // 응답 플래그 등 설정
    if (tud_vendor_write_available() > PKT_SIZE) {
        tud_vendor_write(g_accum_buf, PKT_SIZE);
         tud_vendor_write_flush(); // 빈번한 플러시는 성능 저하 원인
    }

    // 다음 패킷 조립을 위해 초기화
    g_accum_cnt = 0;
}
#endif

#ifdef ECHO
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
#endif

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
