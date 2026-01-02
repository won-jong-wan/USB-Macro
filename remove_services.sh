#!/bin/bash

# ==========================================
# STM32 Service Uninstaller
# ==========================================

# 루트 권한 확인
if [ "$EUID" -ne 0 ]; then
  echo "Error: 이 스크립트는 root 권한으로 실행해야 합니다."
  echo "사용법: sudo ./remove_services.sh"
  exit 1
fi

echo ">>> 서비스 제거 및 정리를 시작합니다..."

# 제거할 서비스 목록 (의존성 역순)
SERVICES=("usb_app.service" "cdc_mode.service" "load-stm32-driver.service")

# 1. 서비스 중지 및 비활성화
echo "1. 서비스 중지(Stop) 및 비활성화(Disable) 중..."
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

# 2. 서비스 파일 삭제
echo "2. 서비스 설정 파일 삭제 중..."
for SERVICE in "${SERVICES[@]}"; do
    FILE_PATH="/etc/systemd/system/$SERVICE"
    if [ -f "$FILE_PATH" ]; then
        echo "  - Removing $FILE_PATH"
        rm "$FILE_PATH"
    else
        echo "  - $FILE_PATH 파일을 찾을 수 없음 (이미 삭제됨)"
    fi
done

# 3. Systemd 데몬 리로드 및 실패 기록 초기화
echo "3. Systemd 데몬 리로드 및 정리..."
systemctl daemon-reload
systemctl reset-failed

# 4. (선택사항) 커널 모듈 강제 제거 확인
# 서비스가 중지될 때 ExecStop이 실행되지만, 만약을 위해 확인
if lsmod | grep -q "^dev"; then
    echo "4. 'dev' 커널 모듈이 남아있어 강제로 제거합니다."
    rmmod dev
else
    echo "4. 'dev' 커널 모듈이 정상적으로 언로드되었습니다."
fi

echo "========================================================"
echo "                   제거 완료"
echo "========================================================"
