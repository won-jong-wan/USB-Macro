#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>
#include "monitor.h"

#define TARGET_DRIVER_NAME "team_own_stm32"
#define TARGET_DEV_PATH "/dev/team_own_stm32"

struct MonitorContext {
    struct udev *udev;
    struct udev_monitor *mon;
    int fd;
};

MonitorContext* monitor_init(void) {
    MonitorContext *ctx = malloc(sizeof(MonitorContext));
    if (!ctx) return NULL;

    ctx->udev = udev_new();
    if (!ctx->udev) { free(ctx); return NULL; }

    ctx->mon = udev_monitor_new_from_netlink(ctx->udev, "udev");
    
    // [검증된 로직] USB 인터페이스 필터링
    udev_monitor_filter_add_match_subsystem_devtype(ctx->mon, "usb", "usb_interface");
    udev_monitor_enable_receiving(ctx->mon);

    ctx->fd = udev_monitor_get_fd(ctx->mon);
    printf(">> [MONITOR] Udev 감시 시작 (Target: %s)\n", TARGET_DRIVER_NAME);
    
    return ctx;
}

int monitor_get_fd(MonitorContext *ctx) {
    return ctx ? ctx->fd : -1;
}

int monitor_process_event(MonitorContext *ctx, MonitorEventType *event_out, char *dev_node_out, int max_len) {
    if (!ctx || !ctx->mon) return -1;

    struct udev_device *dev = udev_monitor_receive_device(ctx->mon);
    if (!dev) return 0;

    const char *action = udev_device_get_action(dev);
    const char *driver = udev_device_get_property_value(dev, "DRIVER");

    *event_out = MONITOR_EVENT_NONE;

    if (action && driver && strcmp(driver, TARGET_DRIVER_NAME) == 0) {
        if (strcmp(action, "bind") == 0) {
            *event_out = MONITOR_EVENT_ADD;
            // 실제 노드 경로를 전달 (우리는 고정 경로 사용)
            strncpy(dev_node_out, TARGET_DEV_PATH, max_len - 1);
        }
        else if (strcmp(action, "unbind") == 0) {
            *event_out = MONITOR_EVENT_REMOVE;
            strncpy(dev_node_out, TARGET_DEV_PATH, max_len - 1);
        }
    }

    udev_device_unref(dev);
    return 0;
}

void monitor_cleanup(MonitorContext *ctx) {
    if (!ctx) return;
    udev_monitor_unref(ctx->mon);
    udev_unref(ctx->udev);
    free(ctx);
}