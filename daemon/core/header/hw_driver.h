#ifndef HW_DRIVER_H
#define HW_DRIVER_H

#include "common.h" // datapacket 구조체 사용

// 드라이버 상태 관리용 구조체 (내부 데이터는 숨김)
typedef struct HWContext HWContext;

// 1. 컨텍스트 초기화/해제
HWContext* hw_init(void);
void hw_cleanup(HWContext *ctx);

// 2. 장치 연결 (성공 0, 실패 -1)
// path는 monitor가 알려준 경로 ("/dev/team_own_stm32" 등)
int hw_connect(HWContext *ctx, const char *path);

// 3. 장치 연결 해제
void hw_disconnect(HWContext *ctx);

// 4. 데이터 읽기 (성공 1, 데이터 없음 0, 치명적 에러/끊김 -1)
int hw_read_packet(HWContext *ctx, datapacket *pkt);

// 5. Poll을 위한 파일 디스크립터 반환 (연결 안 됐으면 -1)
int hw_get_fd(HWContext *ctx);

#endif