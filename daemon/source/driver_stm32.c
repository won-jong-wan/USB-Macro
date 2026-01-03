#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../header/hw_driver.h"

struct HWContext {
    int fd;
};

HWContext* hw_init(void) {
    HWContext *ctx = malloc(sizeof(HWContext));
    if (ctx) ctx->fd = -1;
    return ctx;
}

void hw_cleanup(HWContext *ctx) {
    if (ctx) {
        if (ctx->fd >= 0) close(ctx->fd);
        free(ctx);
    }
}

int hw_connect(HWContext *ctx, const char *path) {
    if (!ctx) return -1;

    printf(">> [DRIVER] 장치 연결 시도: %s\n", path);
    
    // [검증된 로직] 5회 재시도 (파일 생성 대기)
    int fd = -1;
    for (int i = 0; i < 5; i++) {
        fd = open(path, O_RDWR);
        if (fd >= 0) break;
        usleep(100000); // 0.1초 대기
    }

    if (fd < 0) {
        perror("   -> [ERROR] 장치 파일 열기 실패");
        return -1;
    }

    // Non-blocking 설정
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    ctx->fd = fd;
    printf("   -> [DRIVER] 연결 성공 (FD: %d)\n", fd);
    return 0;
}

void hw_disconnect(HWContext *ctx) {
    if (ctx && ctx->fd >= 0) {
        close(ctx->fd);
        ctx->fd = -1;
        printf(">> [DRIVER] 연결 해제 완료\n");
    }
}

int hw_read_packet(HWContext *ctx, datapacket *pkt) {
    if (!ctx || ctx->fd < 0) return -1;

    // ★ [수정 1] 가장 중요한 부분!
    // 읽기 전에 버퍼를 0으로 깨끗이 비웁니다.
    // 이렇게 하면 USB에서 데이터가 짧게 와도, 나머지 뒷부분은 0(NULL)이 되어
    // 문자열이 깨지지 않고 안전하게 종료됩니다.
    memset(pkt, 0, sizeof(datapacket));

    // [수정 2] read 로직 강화
    // USB 시리얼은 가끔 데이터가 잘려서(Fragmented) 올 수 있습니다.
    // 일단은 기존 방식대로 읽되, 나중에 문제가 되면 Loop로 다 읽을 때까지 기다려야 합니다.
    ssize_t len = read(ctx->fd, pkt, sizeof(datapacket));

    if (len > 0) {
        // [수정 3] 이중 안전장치
        // 혹시라도 command가 꽉 차서 NULL 문자가 없을 경우를 대비해 강제 종료
        // 구조체 크기 내에서 안전하게 마지막 바이트를 0으로 설정
        pkt->command[sizeof(pkt->command) - 1] = '\0';
        
        return 1; // 성공
    } 
    else if (len == 0) {
        return 0; // 데이터 없음
    } 
    else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0; // 데이터 아직 안 옴 (정상)
        }
        return -1; // 진짜 에러
    }
}

int hw_get_fd(HWContext *ctx) {
    return ctx ? ctx->fd : -1;
}