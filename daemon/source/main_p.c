#include <stdio.h>
#include <stdlib.h> // atoi 사용
#include <poll.h>
#include <unistd.h>
#include "common.h"
#include "usb_sim.h"
#include "executor.h" // [추가] 실행기 모듈

void handle_packet(datapacket *pkt) {
    // 1. 패킷 정보 출력 (사용자 요청 스타일 유지)
    printf("\n╔════ [패킷 수신 성공] ════════════════════╗\n");
    printf("║ Magic   : 0x%08X %s\n", pkt->magic, (pkt->magic == MAGIC_NUMBER) ? "✔" : "✘ (Error)");
    printf("║ Info    : 0x%02X\n", pkt->info);
    printf("║  ├─ Ver : %d\n", GET_VERSION(pkt->info));
    printf("║  ├─ Delay : %s\n", GET_DELAY(pkt->info) ? "Yes" : "No");
    printf("║  ├─ Start : %s\n", GET_START(pkt->info) ? "True" : "False");
    printf("║  ├─ IsEnd : %s\n", GET_ISEND(pkt->info) ? "Yes" : "No");
    printf("║  └─ Type  : %s\n", GET_TYPE(pkt->info) ? "S-Node" : "C-Node");
    printf("║ Cmd Len : %d\n", pkt->cmd_len);
    printf("║ Command : \"%s\"\n", pkt->command);
    printf("╚══════════════════════════════════════════╝\n");

    // 2. 매직 넘버 검증
    if (pkt->magic != MAGIC_NUMBER) {
        printf(">> [ERROR] 매직 넘버 불일치! 명령을 무시합니다.\n");
        goto PROMPT;
    }

    // 3. 지연(Delay) 처리
    // Delay 비트가 1이면, command에 들어있는 숫자가 초(sec) 단위라고 가정
    if (GET_DELAY(pkt->info)) {
        int delay_sec = atoi(pkt->command);
        printf(">> [DELAY] %d초간 대기합니다...\n", delay_sec);
        sleep(delay_sec);
        printf(">> [DELAY] 대기 완료.\n");
        goto PROMPT;
    }

    // 4. 실행 (Type에 따라 분기)
    // Type 1: S-Node (Foreground, 대기함)
    // Type 0: C-Node (Background, 대기 안 함)
    int is_bg = (GET_TYPE(pkt->info) == 0); 
    execute_command(pkt->command, is_bg);

PROMPT:
    printf("\n(명령어 대기중... a/r/q): ");
    fflush(stdout);
}

// ... (나머지 handle_user_input, main 함수는 기존과 동일) ...
void handle_user_input(SimContext *ctx) {
    char cmd;
    if (read(STDIN_FILENO, &cmd, 1) <= 0) return;

    switch (cmd) {
        case 'a': sim_connect(ctx); break;
        case 'r': sim_disconnect(ctx); break;
        case 'q': ctx->running = 0; break;
    }
}

int main() {
    SimContext ctx;

    if (init_executor() < 0) {
        fprintf(stderr, "프로그램을 종료합니다. (sudo로 실행 필요)\n");
        return 1;
    }

    if (sim_init(&ctx) < 0) return 1;

    printf("(명령어 대기중... a/r/q): ");
    fflush(stdout);

    while (ctx.running) {
        struct pollfd fds[2];
        int nfds = 1;

        // 표준 입력 감시
        // TODO: udev로 USB 감시하는 것으로 바꿀 것
        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        // 장치 연결됐을 시 입력이 왔는지 감시 추가
        if (ctx.fd_dev >= 0) {
            fds[1].fd = ctx.fd_dev;
            fds[1].events = POLLIN;
            nfds = 2;
        }

        // 감시 대기
        if (poll(fds, nfds, -1) < 0) break;

        // USB 장치 또는 표준 입력 처리
        // ctx.fd_dev에 대상 장치 파일 디스크립터 넣음
        if (fds[0].revents & POLLIN) handle_user_input(&ctx);
        
        // 장치에서 데이터 수신 처리
        // 대상 장치 디스크립터에서 pkt 읽어서 handle_packet 호출
        if (nfds == 2 && (fds[1].revents & POLLIN)) {
            datapacket pkt;
            if (sim_read_packet(&ctx, &pkt) == 1) handle_packet(&pkt);
        }
    }
    sim_cleanup(&ctx);
    return 0;
}
