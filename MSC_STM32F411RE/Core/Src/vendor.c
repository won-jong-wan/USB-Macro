/*
 * vendor.c
 *
 *  Created on: Dec 23, 2025
 *      Author: jonwo
 */
#include <main.h>
#include <buffer_event.h>
#include <tusb.h>

#include <string.h>
#include <stdio.h>

uint8_t g_accum_buf[512]__attribute__((aligned(4))); // 조각난 데이터를 합칠 공간
uint32_t g_accum_cnt = 0; // 현재 모인 개수
uint32_t g_ven_header;

uint8_t info_buf[512]__attribute__((aligned(4)));
uint32_t g_ven_info_addr;

void vendor_info_update(int len) {
	printf("-------------\n[vendor] info update\n");

	venpack_t *init_pack = (venpack_t*) info_buf;

	init_pack->magic = 0xBEEFCAFE;
	init_pack->info = 0x1C;
	const char *com = "info_struct";
	init_pack->cmd_len = len;
	memcpy(init_pack->command, com, strlen(com) + 1);

	SD_Write_DMA_Async(g_ven_info_addr, info_buf, 1); // 정보 저장용
	printf("[vendor] info len: %d addr: %ld\n-------------\n\n",
			init_pack->cmd_len, g_ven_info_addr);
}

// 매번 SD 카드 괴롭히지 말고 버퍼에 모았다 다 넣어버리는 것도 괜찮지 않을까..?
// 하지만 버퍼 용량이 꽉찰까봐 + 넣는 시간이 길어지면 다음 통신 못 받을까 두려움
// SDIO 통신 점유 VS 버퍼
void tud_vendor_rx_cb(uint8_t itf, uint8_t const *buffer, uint32_t bufsize) {
	// 1. 패킷 조립 (기존 로직)
	if ((g_accum_cnt + bufsize) > VENPACK_SIZE) {
		printf("[vendor] rx buffer overflow(Vendor faster than SD input)\n");
		// 만약 이 에러가 자주 나면 송신 쪽에 기다리라는 메시지 보낼 필요가 있음
		return;
	};

	memcpy(&g_accum_buf[g_accum_cnt], buffer, bufsize);
	g_accum_cnt += bufsize;

	printf("[vendor] read %ldbyte data\n", g_accum_cnt);

	if (g_accum_cnt < VENPACK_SIZE) {
		return;
	} // 아직 256바이트 안 됨

	// --------
#ifdef ASYNC
    // SD 카드 바쁜지 체크
	if(SD_Check_State() != SD_STATE_READY || SD_Check_Card_State() != SD_STATE_READY){
	    if (tud_vendor_write_available() > VENPACK_SIZE) {
	    	// PC에 다시 보내라고 응답
	    	printf("[vendor] sd busy\n");
	        tud_vendor_write(g_accum_buf, VENPACK_SIZE);
	        tud_vendor_write_flush();
	    }
	}
#endif // ASYNC
	// --------

	// 2. 패킷 완성! (256바이트)
	venpack_t *pkt = (venpack_t*) g_accum_buf;

	print_venpack(pkt);

	SD_Write_DMA_Async(g_ven_header++, g_accum_buf, 1); // 채운 거 보내고
	printf("[vendor] saved sd addr: %ld\n-------------\n\n", g_ven_header - 1);

	g_accum_cnt = 0;

	vendor_info_update(g_ven_header - g_vendor_start_address -1);
}

void ven_send(void) {
	// info check
	printf("-------------\n[vendor] info check\n");
	SD_Read_DMA_Async(g_ven_info_addr, info_buf, 1);

	venpack_t *init_pack = (venpack_t*) info_buf;
	uint16_t send_cnt = init_pack->cmd_len;

	printf("cmd: %s send_cnt: %d\n", init_pack->command, send_cnt);

	for (int i = 0; i < send_cnt; i++) {
		printf("read venpack: %ld\n", g_ven_info_addr+i+1);
		SD_Read_DMA_Async(g_ven_info_addr+i+1, g_accum_buf, 1);

		while (tud_vendor_write_available() <= VENPACK_SIZE) {
			tud_task();
		}
		tud_vendor_write(g_accum_buf, VENPACK_SIZE);
		tud_vendor_write_flush();
	}

	g_ven_header = g_vendor_start_address+1;
	vendor_info_update(0);
}

