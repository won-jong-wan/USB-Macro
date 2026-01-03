#ifndef MONITOR_H
#define MONITOR_H

// 감지된 이벤트 타입
typedef enum {
    MONITOR_EVENT_NONE = 0,
    MONITOR_EVENT_ADD,
    MONITOR_EVENT_REMOVE
} MonitorEventType;

typedef struct MonitorContext MonitorContext;

// 초기화 (Udev 설정)
MonitorContext* monitor_init(void);

// Poll 감시용 FD 반환
int monitor_get_fd(MonitorContext *ctx);

// 이벤트 처리 및 장치 경로 반환
// dev_node_out 버퍼에 "/dev/team_own_stm32" 문자열을 채워줌
int monitor_process_event(MonitorContext *ctx, MonitorEventType *event_out, char *dev_node_out, int max_len);

// 정리
void monitor_cleanup(MonitorContext *ctx);

#endif