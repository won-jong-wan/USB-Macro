#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>

#include "common.h"
#include "executor.h"
#include "monitor.h"
#include "hw_driver.h"

// 전역 변수 (Signal 핸들러에서 정리하기 위해)
static MonitorContext *mon_ctx = NULL;
static HWContext *hw_ctx = NULL;
static volatile int keep_running = 1;

// Ctrl+C 종료 처리
void signal_handler(int signo) {
    keep_running = 0;
}

// 패킷 처리 로직 (기존과 동일, 비즈니스 로직)
void handle_packet(datapacket *pkt) {
    printf("\n╔════ [패킷 수신 성공] ════════════════════╗\n");
    printf("║ Magic   : 0x%08X %s\n", pkt->magic, (pkt->magic == MAGIC_NUMBER) ? "✔" : "✘");
    printf("║ Info    : 0x%02X (Ver:%d, Type:%s)\n", pkt->info, GET_VERSION(pkt->info), GET_TYPE(pkt->info) ? "S" : "C");
    printf("║ Command : \"%.50s\"\n", pkt->command);
    printf("╚══════════════════════════════════════════╝\n");

    if (pkt->magic != MAGIC_NUMBER) {
        printf(">> [WARN] 매직 넘버 불일치. 무시합니다.\n");
        return;
    }

    if (GET_DELAY(pkt->info)) {
        int delay_sec = atoi(pkt->command);
        printf(">> [DELAY] %d초 대기...\n", delay_sec);
        sleep(delay_sec);
    }

    int is_bg = (GET_TYPE(pkt->info) == 0); 
    execute_command(pkt->command, is_bg);
}

int main() {
    // ★ [추가] 출력 버퍼링 끄기 (로그 즉시 출력)
    // 이 코드가 없으면 journalctl에서 로그가 한참 뒤에 뜹니다.
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0); // 에러 출력도 즉시 나오게 설정
    
    // 1. 초기화
    signal(SIGINT, signal_handler); // Ctrl+C 감지

    if (init_executor() < 0) {
        fprintf(stderr, ">> [ERROR] sudo 권한이 필요합니다.\n");
        return 1;
    }

    mon_ctx = monitor_init();
    hw_ctx = hw_init();

    if (!mon_ctx || !hw_ctx) {
        fprintf(stderr, ">> [ERROR] 모듈 초기화 실패\n");
        return 1;
    }

    printf("==========================================\n");
    printf("   RPI Driver Automation System (DAEMON)  \n");
    printf("==========================================\n");
    printf(">> [SYSTEM] 장치 감시 중... (USB를 연결하세요)\n");

    // 2. 메인 루프 (Poll)
    while (keep_running) {
        struct pollfd fds[2];
        int nfds = 1;

        // [0] Udev 감시 (항상 존재)
        fds[0].fd = monitor_get_fd(mon_ctx);
        fds[0].events = POLLIN;

        // [1] 장치 통신 (연결되었을 때만 추가)
        int dev_fd = hw_get_fd(hw_ctx);
        if (dev_fd >= 0) {
            fds[1].fd = dev_fd;
            fds[1].events = POLLIN | POLLHUP | POLLERR; // 끊김 감지 포함
            nfds = 2;
        }

        // 대기 (타임아웃 -1: 무한 대기)
        if (poll(fds, nfds, -1) < 0) {
            if (keep_running) perror("Poll Error");
            break;
        }

        // -----------------------------------------------------------
        // (A) Udev 이벤트 처리 (USB 연결/해제 감지)
        // -----------------------------------------------------------
        if (fds[0].revents & POLLIN) {
            MonitorEventType event;
            char dev_path[64];
            
            monitor_process_event(mon_ctx, &event, dev_path, sizeof(dev_path));

            if (event == MONITOR_EVENT_ADD) {
                printf(">> [EVENT] 장치 감지됨! (%s)\n", dev_path);
                hw_connect(hw_ctx, dev_path); // 연결 시도
            } 
            else if (event == MONITOR_EVENT_REMOVE) {
                printf(">> [EVENT] 장치 제거됨 (Udev).\n");

                terminate_background_jobs();

                hw_disconnect(hw_ctx);
            }
        }

        // -----------------------------------------------------------
        // (B) 데이터 수신 처리 (실제 데이터)
        // -----------------------------------------------------------
        if (nfds == 2) {
            // (B-1) 에러 혹은 끊김 발생
            if (fds[1].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                printf(">> [EVENT] 장치 연결 끊김 (POLLHUP).\n");
                terminate_background_jobs(); // ★ 여기도 추가
                hw_disconnect(hw_ctx);
            }
            // (B-2) 데이터 도착
            else if (fds[1].revents & POLLIN) {
                datapacket pkt;
                int ret = hw_read_packet(hw_ctx, &pkt);

                if (ret == 1) {
                    handle_packet(&pkt); // 패킷 처리
                } else if (ret == -1) {
                    printf(">> [ERROR] 읽기 오류. 연결을 닫습니다.\n");
                    hw_disconnect(hw_ctx);
                }
            }
        }
    }

    // 3. 종료 정리
    printf("\n>> [SYSTEM] 시스템 종료.\n");
    hw_cleanup(hw_ctx);
    monitor_cleanup(mon_ctx);
    return 0;
}