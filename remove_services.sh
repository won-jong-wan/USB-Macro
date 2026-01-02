#!/bin/bash

# ==========================================
# STM32 Service & Udev Uninstaller
# ==========================================

# 루트 권한 확인
if [ "$EUID" -ne 0 ]; then
  echo "Error: 이 스크립트는 root 권한으로 실행해야 합니다."
  echo "사용법: sudo ./remove_services.sh"
  exit 1
fi

echo ">>> 서비스 및 설정 제거를 시작합니다..."

# 제거할 대상 목록
SERVICES=("usb_app.service" "cdc_mode.service" "load-stm32-driver.service")
# 삭제할 udev 규칙 파일들 (버전별 파일명 모두 포함)
UDEV_RULES=("/etc/udev/rules.d/99-stm32-custom.rules" "/etc/udev/rules.d/99-stm32-perm.rules")

# -------------------------------------------------------
# 1. 서비스 중지 및 비활성화
# -------------------------------------------------------
echo "1. 서비스 중지(Stop) 및 비활성화(Disable)..."
for SERVICE in "${SERVICES[@]}"; do
    if systemctl is-active --quiet "$SERVICE"; then
        echo "  - Stopping $SERVICE..."
        systemctl stop "$SERVICE"
    fi
    
    if systemctl is-enabled --quiet "$SERVICE"; then
        echo "  - Disabling $SERVICE..."
        systemctl disable "$SERVICE"
    fi
done

# -------------------------------------------------------
# 2. 서비스 파일 삭제
# -------------------------------------------------------
echo "2. 서비스 설정 파일 삭제..."
for SERVICE in "${SERVICES[@]}"; do
    FILE_PATH="/etc/systemd/system/$SERVICE"
    if [ -f "$FILE_PATH" ]; then
        echo "  - Removing $FILE_PATH"
        rm "$FILE_PATH"
    fi
done

# -------------------------------------------------------
# 3. Udev 규칙 파일 삭제
# -------------------------------------------------------
echo "3. Udev 규칙 파일 삭제..."
for RULE in "${UDEV_RULES[@]}"; do
    if [ -f "$RULE" ]; then
        echo "  - Removing $RULE"
        rm "$RULE"
    fi
done

# -------------------------------------------------------
# 4. 시스템 리로드 (Systemd & Udev)
# -------------------------------------------------------
echo "4. 시스템 데몬 리로드..."
systemctl daemon-reload
systemctl reset-failed

echo "5. Udev 규칙 리로드..."
udevadm control --reload-rules
# 규칙이 사라졌음을 적용 (선택사항)
udevadm trigger

# -------------------------------------------------------
# 5. 커널 모듈 강제 제거
# -------------------------------------------------------
echo "6. 커널 모듈 언로드 확인..."

# dev_struct (새 버전) 제거
if lsmod | grep -q "^dev_struct"; then
    echo "  - Unloading module: dev_struct"
    rmmod dev_struct
fi

# dev (구 버전) 제거
if lsmod | grep -q "^dev "; then
    echo "  - Unloading module: dev"
    rmmod dev
fi

echo "========================================================"
echo "                   제거 완료"
echo "========================================================"
echo "모든 서비스, 설정 파일, udev 규칙이 삭제되었습니다."
