#!/bin/bash

# ==========================================
# STM32 Multi-Process Service Installer
# (Auto-detect Path Version)
# ==========================================

# 루트 권한 확인
if [ "$EUID" -ne 0 ]; then
  echo "Error: 이 스크립트는 root 권한으로 실행해야 합니다."
  echo "사용법: sudo ./install_services.sh"
  exit 1
fi

# -------------------------------------------------------
# [중요] 현재 스크립트의 위치를 기준으로 작업 경로 자동 설정
# -------------------------------------------------------
# 스크립트가 위치한 절대 경로 추출
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# bg_app 폴더 경로 설정
TARGET_DIR="${SCRIPT_DIR}/bg_app"

echo ">>> 설치 경로를 자동으로 감지했습니다: ${TARGET_DIR}"

# 파일 존재 여부 확인 (에러 방지)
if [ ! -f "${TARGET_DIR}/dev.ko" ] || [ ! -f "${TARGET_DIR}/usb_app" ]; then
    echo "Error: '${TARGET_DIR}' 위치에서 'dev.ko' 또는 'usb_app' 파일을 찾을 수 없습니다."
    echo "폴더 구조가 올바른지 확인해주세요."
    exit 1
fi

echo ">>> 서비스 등록 자동화 스크립트를 시작합니다..."

# -------------------------------------------------------
# 1. load-stm32-driver.service 생성
# -------------------------------------------------------
echo "1. Creating load-stm32-driver.service..."
# 주의: 변수($TARGET_DIR)가 들어가므로 경로 부분이 바뀝니다.
cat <<EOF > /etc/systemd/system/load-stm32-driver.service
[Unit]
Description=Load STM32 USB Driver
DefaultDependencies=no
Before=cdc_mode.service usb_app.service

[Service]
Type=oneshot
ExecStartPre=-/usr/sbin/rmmod dev
ExecStart=/usr/sbin/insmod ${TARGET_DIR}/dev.ko
ExecStartPost=/bin/sh -c 'for i in \$(seq 1 10); do [ -e /dev/team_own_stm32 ] && break || sleep 0.5; done'
ExecStartPost=/bin/chmod 666 /dev/team_own_stm32
ExecStop=/usr/sbin/rmmod dev
RemainAfterExit=yes
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=sysinit.target
EOF

# -------------------------------------------------------
# 2. cdc_mode.service 생성 (경로 의존성 없음)
# -------------------------------------------------------
echo "2. Creating cdc_mode.service..."
cat <<EOF > /etc/systemd/system/cdc_mode.service
[Unit]
Description=CDC Serial Console on ttyACM0
Documentation=man:agetty(8)
After=load-stm32-driver.service systemd-user-sessions.service
Before=usb_app.service
Requires=load-stm32-driver.service
ConditionPathExists=/dev/ttyACM0

[Service]
Type=idle
ExecStart=-/sbin/agetty --autologin pi --local-line --noclear -L 115200 ttyACM0 vt100
Restart=always
RestartSec=5
StandardInput=tty
StandardOutput=tty
TTYPath=/dev/ttyACM0
TTYReset=yes
TTYVHangup=yes
TTYVTDisallocate=yes
SyslogIdentifier=cdc_mode

[Install]
WantedBy=multi-user.target
EOF

# -------------------------------------------------------
# 3. usb_app.service 생성
# -------------------------------------------------------
echo "3. Creating usb_app.service..."
cat <<EOF > /etc/systemd/system/usb_app.service
[Unit]
Description=STM32 Multi-Process Command Execution System
After=load-stm32-driver.service cdc_mode.service
Requires=load-stm32-driver.service
BindsTo=dev-team_own_stm32.device

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
# 4. 서비스 등록 및 시작
# -------------------------------------------------------
echo ">>> Systemd 데몬 리로드 중..."
systemctl daemon-reload

echo ">>> 서비스 활성화 (Enable) 중..."
systemctl enable load-stm32-driver.service
systemctl enable cdc_mode.service
systemctl enable usb_app.service

echo ">>> 서비스 시작 (Start) 중..."
systemctl start load-stm32-driver.service
systemctl start cdc_mode.service
systemctl start usb_app.service

# -------------------------------------------------------
# 5. 상태 확인
# -------------------------------------------------------
echo "========================================================"
echo "                   설치 완료"
echo "========================================================"
echo "설치된 경로: ${TARGET_DIR}"
echo "--------------------------------------------------------"
echo "[1] load-stm32-driver:" $(systemctl is-active load-stm32-driver.service)
echo "[2] cdc_mode:         " $(systemctl is-active cdc_mode.service)
echo "[3] usb_app:          " $(systemctl is-active usb_app.service)
echo "--------------------------------------------------------"
