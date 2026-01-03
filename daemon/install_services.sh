#!/bin/bash

# ==========================================
# STM32 Service Installer (Daemon Version)
# ==========================================

if [ "$EUID" -ne 0 ]; then
  echo "Error: Run as root (sudo ./install_services.sh)"
  exit 1
fi

# -------------------------------------------------------
# [설정] 프로젝트 경로 및 파일명
# -------------------------------------------------------
# 현재 스크립트가 실행되는 경로 (프로젝트 루트)
PROJECT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# 우리가 만든 C 프로그램 (Makefile 결과물)
SOURCE_BIN="${PROJECT_ROOT}/build/sim"
# 커널 모듈 (경로에 맞게 수정 필요, 예: 현재 폴더의 dev_struct.ko)
MODULE_FILE="${PROJECT_ROOT}/dev_struct.ko" 
MODULE_NAME="dev_struct"

# 설치될 경로 (이곳으로 파일들을 복사해서 관리)
INSTALL_DIR="/usr/local/stm32_service"
TARGET_BIN="${INSTALL_DIR}/usb_app"

# -------------------------------------------------------
# [설정] 사용자 정보 및 USB ID
# -------------------------------------------------------
DEFAULT_USER=${SUDO_USER:-"pi"}
read -p "사용자 ID (기본값: ${DEFAULT_USER}): " INPUT_USER
LOGIN_USER=${INPUT_USER:-$DEFAULT_USER}

# CDC (PID: DEAD)
CDC_VID="cafe"
CDC_PID="dead"
CDC_SYMLINK="ttySTM32"

# MSC + App (PID: 4000)
APP_VID="cafe"
APP_PID="4000"
MOUNT_POINT="/mnt/usb-macro"

echo ">>> 설치 시작 (Daemon Mode)..."

# 0. 설치 폴더 생성 및 파일 복사
echo "0. Installing Binaries..."
mkdir -p ${INSTALL_DIR}

# (1) 실행 파일 복사 (build/sim -> /usr/local/stm32_service/usb_app)
if [ -f "${SOURCE_BIN}" ]; then
    cp "${SOURCE_BIN}" "${TARGET_BIN}"
    chmod +x "${TARGET_BIN}"
    echo "   -> App installed to ${TARGET_BIN}"
else
    echo "   [Error] ${SOURCE_BIN} 파일이 없습니다. 'make'를 먼저 실행하세요."
    exit 1
fi

# (2) 모듈 파일 복사
if [ -f "${MODULE_FILE}" ]; then
    cp "${MODULE_FILE}" "${INSTALL_DIR}/${MODULE_NAME}.ko"
    echo "   -> Module copied to ${INSTALL_DIR}"
else
    echo "   [Warning] ${MODULE_FILE} 파일이 없습니다. 모듈 로드 서비스가 실패할 수 있습니다."
fi

# -------------------------------------------------------
# 1. MSC 마운트 도우미 (변경 없음)
# -------------------------------------------------------
echo "1. Creating MSC Mount Helper..."
cat <<EOF > /usr/local/bin/stm32_mounter.sh
#!/bin/bash
DEV_NAME=\$1
LOGfile="/tmp/stm32_mount.log"
/usr/bin/systemd-mount --no-block --collect --type=vfat --options "uid=\$(id -u ${LOGIN_USER}),gid=\$(id -g ${LOGIN_USER}),utf8,dmask=000,fmask=000" --fsck=no "/dev/\$DEV_NAME" "${MOUNT_POINT}" >> \$LOGfile 2>&1
EOF
chmod +x /usr/local/bin/stm32_mounter.sh

# -------------------------------------------------------
# 2. Udev 규칙 (수정됨)
# -------------------------------------------------------
echo "2. Updating udev rules..."
mkdir -p "${MOUNT_POINT}"
chown $LOGIN_USER:$LOGIN_USER "${MOUNT_POINT}"
chmod 777 "${MOUNT_POINT}"

# ★ 중요 변경점:
# usb_app.service는 이제 부팅 시 자동 실행되므로, Udev에서 'systemd start'를 걸지 않습니다.
# 대신 마운트 스크립트만 실행합니다.

cat <<EOF > /etc/udev/rules.d/99-stm32-custom.rules
# GROUP B: CDC Device
SUBSYSTEM=="tty", ATTRS{idVendor}=="${CDC_VID}", ATTRS{idProduct}=="${CDC_PID}", SYMLINK+="${CDC_SYMLINK}", TAG+="systemd", ENV{SYSTEMD_WANTS}+="cdc_mode.service"

# GROUP A: MSC Mount Logic Only
# 저장소 감지 -> 마운트
ACTION=="add", SUBSYSTEM=="block", ATTRS{idVendor}=="${APP_VID}", ATTRS{idProduct}=="${APP_PID}", RUN+="/usr/local/bin/stm32_mounter.sh %k"

# 저장소 제거 -> 언마운트
ACTION=="remove", SUBSYSTEM=="block", ENV{ID_VENDOR_ID}=="${APP_VID}", ENV{ID_MODEL_ID}=="${APP_PID}", RUN+="/usr/bin/systemd-mount --umount ${MOUNT_POINT}"
EOF

udevadm control --reload-rules
udevadm trigger

# -------------------------------------------------------
# 3. Systemd 서비스 (핵심 변경!)
# -------------------------------------------------------
echo "3. Creating Services..."

# 3-1. Load Driver (모듈 로드)
cat <<EOF > /etc/systemd/system/load-stm32-driver.service
[Unit]
Description=Load STM32 Custom Driver
DefaultDependencies=no
Before=usb_app.service

[Service]
Type=oneshot
ExecStartPre=-/usr/sbin/rmmod ${MODULE_NAME}
# 설치된 경로의 모듈 로드
ExecStart=/bin/bash -c "/usr/sbin/insmod ${INSTALL_DIR}/${MODULE_NAME}.ko || /usr/sbin/lsmod | grep -q ${MODULE_NAME}"
RemainAfterExit=yes
EOF

# 3-2. CDC Mode (변경 없음)
cat <<EOF > /etc/systemd/system/cdc_mode.service
[Unit]
Description=CDC Serial Console
After=network.target 
StopWhenUnneeded=yes

[Service]
Type=idle
ExecStartPre=/bin/bash -c 'for i in {1..20}; do if [ -e /dev/${CDC_SYMLINK} ]; then exit 0; fi; sleep 0.5; done; exit 1'
ExecStart=-/sbin/agetty --autologin ${LOGIN_USER} --local-line --noclear -L 115200 ${CDC_SYMLINK} vt100
Restart=always
RestartSec=3
EOF

# 3-3. USB App (Environment 추가됨)
cat <<EOF > /etc/systemd/system/usb_app.service
[Unit]
Description=STM32 Automation Daemon
After=load-stm32-driver.service network.target
Wants=load-stm32-driver.service

[Service]
Type=simple
WorkingDirectory=${INSTALL_DIR}
ExecStart=${TARGET_BIN}

# [핵심] C 프로그램에게 "이 서비스의 주인이 누구인지" 알려줌
Environment="DAEMON_USER=${LOGIN_USER}"

Restart=always
RestartSec=5
StandardOutput=journal
StandardError=journal
KillSignal=SIGINT

[Install]
WantedBy=multi-user.target
EOF

# -------------------------------------------------------
# 4. 마무리
# -------------------------------------------------------
systemctl daemon-reload
systemctl reset-failed 

# 기존 서비스 중지
systemctl stop usb_app.service 2>/dev/null
systemctl stop cdc_mode.service 2>/dev/null
systemctl stop load-stm32-driver.service 2>/dev/null

# 서비스 활성화 (부팅 시 자동 실행)
systemctl enable load-stm32-driver.service
systemctl enable usb_app.service 
systemctl enable cdc_mode.service

# 서비스 즉시 시작
echo ">>> 서비스를 시작합니다..."
systemctl start load-stm32-driver.service
systemctl start usb_app.service

echo "========================================================"
echo " [설치 완료]"
echo " 1. 앱 위치: ${TARGET_BIN}"
echo " 2. 로그 확인: journalctl -u usb_app -f"
echo " 3. 이제 PC를 재부팅해도 앱은 항상 실행되며 USB를 기다립니다."
echo "========================================================"