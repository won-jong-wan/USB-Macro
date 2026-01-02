#!/bin/bash

# ==========================================
# STM32 Service Installer (Robust App Ver.)
# ==========================================

if [ "$EUID" -ne 0 ]; then
  echo "Error: Run as root (sudo ./install_services.sh)"
  exit 1
fi

# -------------------------------------------------------
# [설정] 사용자 정보
# -------------------------------------------------------
DEFAULT_USER=${SUDO_USER:-"pi"}
read -p "자동 로그인할 사용자 ID를 입력하세요 (기본값: ${DEFAULT_USER}): " INPUT_USER
LOGIN_USER=${INPUT_USER:-$DEFAULT_USER}

# -------------------------------------------------------
# [설정] 장치 정보
# -------------------------------------------------------

# CDC (PID: DEAD) - 독립
CDC_VID="cafe"
CDC_PID="dead"
CDC_SYMLINK="ttySTM32"

# MSC + App (PID: 4000) - 복합
APP_VID="cafe"
APP_PID="4000"
MOUNT_POINT="/mnt/usb-macro"
MODULE_NAME="dev_struct"
# [중요] 앱이 기다려야 할 드라이버 장치 파일 이름
DEVICE_NODE="team_own_stm32" 

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TARGET_DIR="${SCRIPT_DIR}/bg_app"

echo ">>> 설치 시작 (앱 안정성 강화 버전)..."

# -------------------------------------------------------
# 1. MSC 마운트 도우미 (그대로 유지)
# -------------------------------------------------------
echo "1. Creating MSC Mount Helper..."
cat <<EOF > /usr/local/bin/stm32_mounter.sh
#!/bin/bash
DEV_NAME=\$1
LOGfile="/tmp/stm32_mount.log"
MAX_RETRIES=20 

echo "-----------------------------------" >> \$LOGfile
echo "[\$(date)] MSC 장치(PID:4000) 감지됨: /dev/\$DEV_NAME" >> \$LOGfile

READY=0
for ((i=1; i<=MAX_RETRIES; i++)); do
    if /sbin/blkid "/dev/\$DEV_NAME" | grep -q "vfat"; then
        echo "[준비 완료] \$i번째 시도에 파일 시스템 감지됨." >> \$LOGfile
        READY=1
        break
    fi
    /bin/sleep 0.5
done

if [ \$READY -eq 0 ]; then
    echo "[실패] MSC 인식 시간 초과" >> \$LOGfile
    exit 1
fi

sleep 1

/usr/bin/systemd-mount \\
    --no-block \\
    --collect \\
    --type=vfat \\
    --options "uid=\$(id -u ${LOGIN_USER}),gid=\$(id -g ${LOGIN_USER}),utf8,dmask=000,fmask=000" \\
    --fsck=no \\
    "/dev/\$DEV_NAME" "${MOUNT_POINT}" >> \$LOGfile 2>&1
EOF
chmod +x /usr/local/bin/stm32_mounter.sh

# -------------------------------------------------------
# 2. Udev 규칙 (그대로 유지)
# -------------------------------------------------------
echo "2. Updating udev rules..."
mkdir -p "${MOUNT_POINT}"
chown $LOGIN_USER:$LOGIN_USER "${MOUNT_POINT}"
chmod 777 "${MOUNT_POINT}"

cat <<EOF > /etc/udev/rules.d/99-stm32-custom.rules
# GROUP B: CDC Device (PID: DEAD)
SUBSYSTEM=="tty", ATTRS{idVendor}=="${CDC_VID}", ATTRS{idProduct}=="${CDC_PID}", SYMLINK+="${CDC_SYMLINK}", TAG+="systemd", ENV{SYSTEMD_WANTS}+="cdc_mode.service"

# GROUP A: MSC + App Device (PID: 4000)
# 1. 드라이버가 생성한 장치 노드 감지 -> 앱 실행
KERNEL=="${DEVICE_NODE}", MODE="0666", TAG+="systemd", ENV{SYSTEMD_WANTS}+="usb_app.service"

# 2. 저장소 감지 -> 마운트
ACTION=="add", SUBSYSTEM=="block", ATTRS{idVendor}=="${APP_VID}", ATTRS{idProduct}=="${APP_PID}", RUN+="/usr/local/bin/stm32_mounter.sh %k"

# 3. 저장소 제거 -> 언마운트
ACTION=="remove", SUBSYSTEM=="block", ENV{ID_VENDOR_ID}=="${APP_VID}", ENV{ID_MODEL_ID}=="${APP_PID}", RUN+="/usr/bin/systemd-mount --umount ${MOUNT_POINT}"
EOF

udevadm control --reload-rules
udevadm trigger

# -------------------------------------------------------
# 3. Systemd 서비스 (핵심 수정!)
# -------------------------------------------------------
echo "3. Creating Services..."

# 3-1. Load Driver
# [수정] insmod 실패 시(이미 로드됨 등)에도 성공으로 간주하도록 '|| true' 처리하거나 조건문 사용
cat <<EOF > /etc/systemd/system/load-stm32-driver.service
[Unit]
Description=Load STM32 Custom Driver
DefaultDependencies=no
Before=usb_app.service

[Service]
Type=oneshot
# 기존 모듈 제거 시도 (실패해도 무시 '-')
ExecStartPre=-/usr/sbin/rmmod ${MODULE_NAME}
ExecStartPre=-/usr/sbin/rmmod dev
# 모듈 로드. 실패하면(이미 있어서 등) lsmod로 확인 후 있으면 성공(exit 0) 처리
ExecStart=/bin/bash -c "/usr/sbin/insmod ${TARGET_DIR}/${MODULE_NAME}.ko || /usr/sbin/lsmod | grep -q ${MODULE_NAME}"
RemainAfterExit=yes
StandardOutput=journal
EOF

# 3-2. CDC Mode (독립형 유지)
cat <<EOF > /etc/systemd/system/cdc_mode.service
[Unit]
Description=CDC Serial Console
After=network.target 
StopWhenUnneeded=yes

[Service]
Type=idle
StandardInput=null
StandardOutput=journal
# 링크 생성 대기
ExecStartPre=/bin/bash -c 'for i in {1..20}; do if [ -e /dev/${CDC_SYMLINK} ]; then exit 0; fi; sleep 0.5; done; exit 1'
ExecStart=-/sbin/agetty --autologin ${LOGIN_USER} --local-line --noclear -L 115200 ${CDC_SYMLINK} vt100
Restart=always
RestartSec=3
SyslogIdentifier=cdc_mode

[Install]
WantedBy=multi-user.target
EOF

# 3-3. USB App
# [수정] Requires -> Wants (드라이버 서비스 에러나도 일단 실행 시도)
# [수정] ExecStartPre 추가 (장치 파일 /dev/team_own_stm32 대기)
cat <<EOF > /etc/systemd/system/usb_app.service
[Unit]
Description=STM32 Multi-Process App
After=load-stm32-driver.service
# Requires 대신 Wants 사용: 드라이버 로드 서비스가 꼬여도 앱은 죽지 않고 재시도하게 함
Wants=load-stm32-driver.service
StopWhenUnneeded=yes

[Service]
Type=simple
WorkingDirectory=${TARGET_DIR}

# [핵심] 장치 파일이 생길 때까지 최대 10초 대기 (Segfault 방지)
ExecStartPre=/bin/bash -c 'for i in {1..20}; do if [ -e /dev/${DEVICE_NODE} ]; then exit 0; fi; sleep 0.5; done; exit 1'

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
# 4. 마무리
# -------------------------------------------------------
systemctl daemon-reload
systemctl reset-failed 

systemctl stop usb_app.service 2>/dev/null
systemctl stop cdc_mode.service 2>/dev/null
systemctl stop load-stm32-driver.service 2>/dev/null

# 모듈 강제 제거 (테스트 초기화)
rmmod ${MODULE_NAME} 2>/dev/null

systemctl enable load-stm32-driver.service
systemctl enable usb_app.service cdc_mode.service

# 드라이버 수동 로드 시도
systemctl start load-stm32-driver.service

echo "========================================================"
echo " [설치 완료]"
echo " 1. load-stm32-driver: 모듈이 이미 있어도 성공 처리됨"
echo " 2. usb_app: /dev/${DEVICE_NODE} 생성을 확인하고 실행됨"
echo "========================================================"
