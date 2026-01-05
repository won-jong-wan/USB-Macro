#!/bin/bash

# ==========================================
# STM32 Service Uninstaller (Clean Cleanup)
# ==========================================

if [ "$EUID" -ne 0 ]; then
  echo "Error: Run as root (sudo ./remove_services.sh)"
  exit 1
fi

# -------------------------------------------------------
# [설정] 삭제 대상 정의
# -------------------------------------------------------
MOUNT_POINT="/mnt/usb-macro"
HELPER_SCRIPT="/usr/local/bin/stm32_mounter.sh"
LOG_FILE="/tmp/stm32_mount.log"
SERVICES=("usb_app.service" "cdc_mode.service" "load-stm32-driver.service")
MODULE_NAME="dev_struct"

# [추가됨] 새로 변경된 설치 경로
INSTALL_DIR="/usr/local/stm32_service"

echo ">>> STM32 서비스 및 설정 제거 시작..."

# 1. 서비스 중지 및 삭제
echo "1. Stopping and disabling services..."
for svc in "${SERVICES[@]}"; do
    if systemctl is-active --quiet "$svc"; then
        systemctl stop "$svc"
        echo "  - Stopped $svc"
    fi
    if systemctl is-enabled --quiet "$svc"; then
        systemctl disable "$svc"
        echo "  - Disabled $svc"
    fi
    # 서비스 파일 삭제
    if [ -f "/etc/systemd/system/$svc" ]; then
        rm "/etc/systemd/system/$svc"
        echo "  - Removed /etc/systemd/system/$svc"
    fi
done

# 2. 커널 모듈 언로드
echo "2. Unloading kernel module..."
rmmod "$MODULE_NAME" 2>/dev/null
rmmod dev 2>/dev/null
echo "  - Module unloaded"

# 3. Udev 규칙 삭제
echo "3. Removing udev rules..."
if [ -f "/etc/udev/rules.d/99-stm32-custom.rules" ]; then
    rm "/etc/udev/rules.d/99-stm32-custom.rules"
    echo "  - Removed 99-stm32-custom.rules"
fi
# 혹시 아까 테스트하며 남은 임시 규칙도 삭제
if [ -f "/etc/udev/rules.d/99-stm32-simple.rules" ]; then
    rm "/etc/udev/rules.d/99-stm32-simple.rules"
    echo "  - Removed 99-stm32-simple.rules"
fi

udevadm control --reload-rules
udevadm trigger

# 4. 마운트 관련 정리 (가장 중요)
echo "4. Cleaning up MSC mount settings..."

# 4-1. 마운트 해제 시도 (systemd 방식)
if mountpoint -q "$MOUNT_POINT"; then
    echo "  - Unmounting $MOUNT_POINT..."
    /usr/bin/systemd-mount --umount "$MOUNT_POINT"
    sleep 1
fi

# 4-2. 강제 언마운트 (혹시 위에서 안 빠졌을 경우 대비)
if mountpoint -q "$MOUNT_POINT"; then
    umount -l "$MOUNT_POINT" 2>/dev/null
fi

# 4-3. 마운트 도우미 스크립트 삭제
if [ -f "$HELPER_SCRIPT" ]; then
    rm "$HELPER_SCRIPT"
    echo "  - Removed $HELPER_SCRIPT"
fi

# 4-4. 마운트 포인트 폴더 삭제
if [ -d "$MOUNT_POINT" ]; then
    rmdir "$MOUNT_POINT" 2>/dev/null
    echo "  - Removed directory $MOUNT_POINT"
fi

# 4-5. 로그 파일 정리
if [ -f "$LOG_FILE" ]; then
    rm "$LOG_FILE"
    echo "  - Removed log file $LOG_FILE"
fi

# -------------------------------------------------------
# [추가됨] 5. 설치된 프로그램 폴더 삭제
# -------------------------------------------------------
echo "5. Removing Application Directory..."
if [ -d "$INSTALL_DIR" ]; then
    rm -rf "$INSTALL_DIR"
    echo "  - Removed installation directory: $INSTALL_DIR"
fi

# 6. Systemd 리로드
systemctl daemon-reload
systemctl reset-failed

echo "========================================================"
echo " [제거 완료]"
echo " 시스템이 초기 상태로 복구되었습니다."
echo "========================================================"