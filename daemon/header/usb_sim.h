#ifndef USB_SIM_H
#define USB_SIM_H

#include "common.h" // 공통 구조체 사용

// === [설정 및 프로토콜 정의] ===
// 시뮬레이션용 경로지만, 실제 코드에선 필요 없거나 무시됨
#define FAKE_DEVICE "/tmp/fake_usb" 

// 시뮬레이터 상태 객체
typedef struct {
    int fd_dev;     // 장치 파일 디스크립터
    int running;    // 프로그램 실행 플래그
} SimContext;

// === [함수 원형 선언] ===
int sim_init(SimContext *ctx);
void sim_cleanup(SimContext *ctx);
void sim_connect(SimContext *ctx);
void sim_disconnect(SimContext *ctx);

// 데이터 읽기 (성공:1, 실패/없음:0, 에러:-1)
int sim_read_packet(SimContext *ctx, datapacket *out_pkt);

#endif
