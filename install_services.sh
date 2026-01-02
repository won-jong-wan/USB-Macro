#!/bin/bash

# ==========================================
# STM32 Hot-Plug Service Installer
# (Solution: Udev triggers the service)
# ==========================================

if [ "$EUID" -ne 0 ]; then
  echo "Error: Run as root (sudo ./install_services.sh)"
  exit 1
fi

# -------------------------------------------------------
# [설정] 사용자 입력 (로그인 ID)
# -------------------------------------------------------
DEFAULT_USER=${SUDO_USER:-"pi"}

echo "========================================================"
echo " [설정] 시리얼 콘솔(CDC) 자동 로그인 사용자 선택"
echo "========================================================"
read -p "자동 로그인할 사용자 ID를 입력하세요 (기본값: ${DEFAULT_USER}): " INPUT_USER
LOGIN_USER=${INPUT_USER:-$DEFAULT_USER}

echo ">>> 선택된 사용자 ID: ${LOGIN_USER}"

# -------------------------------------------------------
# [설정] 장치 정보
# -------------------------------------------------------
DEVICE_NAME="team_own_stm32"
MODULE_NAME="dev_struct"
CDC_SYMLINK="ttySTM32"

TARGET_VID="cafe"
TARGET_PID="dead"

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TARGET_DIR="${SCRIPT_DIR}/bg_app"

echo ">>> 환경 설정 확인"
echo " - 타겟 ID: VID=${TARGET_VID} / PID=${TARGET_PID}"

if [ ! -f "${TARGET_DIR}/${MODULE_NAME}.ko" ]; then
    echo "Error: ${TARGET_DIR}/${MODULE_NAME}.ko 파일 없음"
    exit 1
fi

echo ">>> 설치 시작..."

# -------------------------------------------------------
# 0. Udev 규칙 (핵심 변경: SYSTEMD_WANTS 추가)
# -------------------------------------------------------
echo "0. Updating udev rules..."
# 이 장치가 감지되면 udev가 자동으로 cdc_mode.service를 실행시킵니다.
cat <<EOF > /etc/udev/rules.d/99-stm32-custom.rules
# [Rule 1] Main Driver Device
KERNEL=="${DEVICE_NAME}", MODE="0666", TAG+="systemd"

# [Rule 2] CDC Serial -> Create Symlink AND Trigger Service
SUBSYSTEM=="tty", ATTRS{idVendor}=="${TARGET_VID}", ATTRS{idProduct}=="${TARGET_PID}", SYMLINK+="${CDC_SYMLINK}", TAG+="systemd", ENV{SYSTEMD_WANTS}+="cdc_mode.service"
EOF

udevadm control --reload-rules
udevadm trigger

# -------------------------------------------------------
# 1. load-stm32-driver.service
# -------------------------------------------------------
echo "1. Creating load-stm32-driver.service..."
cat <<EOF > /etc/systemd/system/load-stm32-driver.service
[Unit]
Description=Load STM32 USB Driver
DefaultDependencies=no
Before=cdc_mode.service usb_app.service

[Service]
Type=oneshot
ExecStartPre=-/usr/sbin/rmmod ${MODULE_NAME}
ExecStartPre=-/usr/sbin/rmmod dev
ExecStart=/usr/sbin/insmod ${TARGET_DIR}/${MODULE_NAME}.ko
ExecStop=/usr/sbin/rmmod ${MODULE_NAME}
RemainAfterExit=yes
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=sysinit.target
EOF

# -------------------------------------------------------
# 2. cdc_mode.service (핵심 변경: 조건 제거, Udev 의존)
# -------------------------------------------------------
echo "2. Creating cdc_mode.service..."
cat <<EOF > /etc/systemd/system/cdc_mode.service
[Unit]
Description=CDC Serial Console on Specific Device (VID:${TARGET_VID} PID:${TARGET_PID})
After=load-stm32-driver.service
Requires=load-stm32-driver.service
# 이제 Udev가 실행을 담당하므로 BindsTo나 ConditionPathExists 제거
# 장치가 뽑히면 agetty가 종료되고 서비스도 자연스럽게 내려감
StopWhenUnneeded=yes

[Service]
Type=idle
# 심볼릭 링크 생성 1초 대기 (안전장치)
ExecStartPre=/bin/sleep 1
StandardInput=null
StandardOutput=journal
StandardError=journal

# agetty 실행
ExecStart=-/sbin/agetty --autologin ${LOGIN_USER} --local-line --noclear -L 115200 ${CDC_SYMLINK} vt100

Restart=always
RestartSec=3
SyslogIdentifier=cdc_mode

[Install]
WantedBy=multi-user.target
EOF

# -------------------------------------------------------
# 3. usb_app.service
# -------------------------------------------------------
echo "3. Creating usb_app.service..."
cat <<EOF > /etc/systemd/system/usb_app.service
[Unit]
Description=STM32 Multi-Process App
After=load-stm32-driver.service cdc_mode.service
Requires=load-stm32-driver.service
BindsTo=dev-${DEVICE_NAME}.device

[Service]
Type=simple
WorkingDirectory=${TARGET_DIR}
ExecStart=${TARGET_DIR}/usb_app
Restart=always
RestartSec=5
StandardOutput=journal
StandardError=journal
KillMode=mixed
KillSignal=SIGTERM
TimeoutStopSec=10
Environment="HOME=${SCRIPT_DIR}"

[Install]
WantedBy=multi-user.target
EOF

# -------------------------------------------------------
# 4. 등록 및 시작
# -------------------------------------------------------
systemctl daemon-reload
systemctl reset-failed 

# 기존에 실패 상태로 죽어있는 서비스 초기화
systemctl stop cdc_mode.service 2>/dev/null

systemctl enable load-stm32-driver.service usb_app.service
# cdc_mode는 udev가 켜주지만, 명시적으로 enable 해두어도 무방함
systemctl enable cdc_mode.service

systemctl start load-stm32-driver.service

echo "========================================================"
echo "설치 완료 (Udev Trigger 방식 적용)"
echo "--------------------------------------------------------"
echo "1. USB 장치를 뽑았다가 다시 연결하세요."
echo "2. 연결 후 2~3초 뒤 아래 명령어로 확인하세요:"
echo "   sudo systemctl status cdc_mode.service"
echo "========================================================"
