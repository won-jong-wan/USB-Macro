#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "usb_sim.h"

int sim_init(SimContext *ctx) {
    ctx->fd_dev = -1;
    ctx->running = 1;

    unlink(FAKE_DEVICE);
    if (mkfifo(FAKE_DEVICE, 0666) < 0) {
        perror("ERROR: mkfifo failed");
        return -1;
    }
    printf("=== [WSL SIMULATION MODULE] ===\n");
    printf(">> Pipe Created: %s\n", FAKE_DEVICE);
    return 0;
}

void sim_cleanup(SimContext *ctx) {
    if (ctx->fd_dev >= 0) close(ctx->fd_dev);
    unlink(FAKE_DEVICE);
    printf(">> Simulation Cleaned up.\n");
}

void sim_connect(SimContext *ctx) {
    if (ctx->fd_dev >= 0) {
        printf(">> [INFO] Already Connected.\n");
        return;
    }
    ctx->fd_dev = open(FAKE_DEVICE, O_RDWR | O_NONBLOCK);
    if (ctx->fd_dev < 0) perror(">> [ERROR] Connection Failed");
    else printf(">> [EVENT] Device Connected (FD: %d)\n", ctx->fd_dev);
}

void sim_disconnect(SimContext *ctx) {
    if (ctx->fd_dev < 0) return;
    printf(">> [EVENT] Device Removed\n");
    close(ctx->fd_dev);
    ctx->fd_dev = -1;
}

int sim_read_packet(SimContext *ctx, datapacket *out_pkt) {
    if (ctx->fd_dev < 0) return -1;

    ssize_t n = read(ctx->fd_dev, out_pkt, sizeof(datapacket));

    if (n == sizeof(datapacket)) return 1; // 성공
    else if (n > 0) {
        printf(">> [WARN] Broken Packet (%ld bytes)\n", n);
        return 0;
    }
    else if (n == 0) return 0; // EOF
    else {
        if (errno == EAGAIN) return 0;
        perror("Read Error");
        return -1;
    }
}
