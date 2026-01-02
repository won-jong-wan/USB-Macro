# USB-MACRO 🚀

**SSH 붙잡고 ROS 환경 세팅하느라 시간 쓰지 말고, USB 하나 꽂고 커피 한 잔 하세요.**  
**USB-MACRO는 TurtleBot(Raspberry Pi)를 “USB 동글(Black Pill)”로 자동 실행/복구까지 하는 Smart USB Bridge 입니다.**

<center>
<img width="1024" height="559" alt="image" src="https://github.com/user-attachments/assets/16ff0826-228a-4b60-b31c-b8ce300529d9" />



---

## 한 문장으로

> **PC에서 “명령 패킷들”만 미리 적재해두고 → 로봇에 꽂으면 → 로봇이 알아서 실행합니다.**  
> 네트워크/SSH가 죽어도 **CDC↔UART 시리얼 콘솔**로 바로 복구합니다.

---

## Before / After

### Before 😵
- 로봇 AP/망 붙이기  
- SSH 접속 → ROS 환경 export  
- 터미널 여러 개 켜서 bringup / 센서 / 노드 실행  
- 네트워크 끊기면… 끝 (현장 멘붕)

### After ☕
- PC에서 버튼 클릭(명령어 적재)
- **Black Pill을 로봇에 꽂기**
- (트리거 한 번) **자동 실행**
- 네트워크가 죽어도 **시리얼 콘솔로 복구**

---

## 어떻게 돌아가요?

핵심은 **Store-and-Forward** 입니다.

1) **STORE (PC)**: PC(Qt)가 256Byte “명령 패킷”을 Vendor로 전송  
2) **STORE (Dongle)**: Black Pill이 받은 패킷을 **SD에 저장**  
3) **RUN (Robot)**: Black Pill을 로봇(RPi)에 연결하면, 저장된 패킷을 Vendor로 다시 보내고(RPi가 수신), daemon이 실행

```mermaid
%%{init: {"themeVariables": {"fontSize": "16px"}, "flowchart": {"useMaxWidth": true, "nodeSpacing": 40, "rankSpacing": 50}}}%%
flowchart LR
  PC["PC<br/>Qt + Kernel Driver"] -->|"Vendor 256Byte<br/>STORE"| MCU["Black Pill<br/>Vendor + SD Store"]
  MCU -->|"Vendor 256Byte<br/>SEND"| RPI["Raspberry Pi<br/>Kernel Driver + daemon"]
  RPI --> RUN["Execute<br/>ROS2 / system cmd"]
```

---

## 2가지 모드

### ✅ MAIN: 자동 실행 (Vendor 256Byte Store-and-Forward)
- PC: `/dev/custom_usb_pc` 로 256Byte 패킷 write
- Dongle: 수신한 패킷을 SD에 저장
- Robot: `/dev/custom_usb_rpi` 로 256Byte 패킷 read → daemon이 S/D/C로 파싱/실행

### 🆘 EMERGENCY: 네트워크 죽어도 복구 (CDC↔UART Serial Console)
- Black Pill이 **CDC 모드**로 전환
- PC는 **가상 COM 포트**로 접속
- CDC 데이터가 UART로 브릿지되어 RPi의 **agetty 시리얼 콘솔**로 연결

```mermaid
%%{init: {"themeVariables": {"fontSize": "16px"}, "flowchart": {"useMaxWidth": true, "nodeSpacing": 35, "rankSpacing": 45}}}%%
flowchart LR
  PC["PC Terminal"] <-->|"USB CDC"| MCU["Black Pill<br/>CDC↔UART Bridge"] <-->|"UART"| RPI["RPi agetty"] --> SHELL["Shell / Recovery"]
```

---

## Quick Start (요약)

> 아래는 “동작 확인”용 최소 흐름입니다. 레포 환경에 맞게 경로만 조정하세요.

### 1) 펌웨어 업로드
```bash
cd firmware
make
# ST-Link/DFU 등으로 flash
```

### 2) 커널 드라이버 로드 (PC / RPi)
```bash
cd kernel_driver
make
sudo insmod custom_usb.ko
ls -l /dev/custom_usb*
```

### 3) PC에서 패킷 적재 (STORE)
```bash
cd pc_client_qt
./CUSTOM_USB_CLIENT
# Qt에서 명령 생성 → /dev/custom_usb_pc로 256Byte 전송
```

### 4) 로봇에서 실행 (RUN)
```bash
cd rpi_daemon
python3 main.py
# 저장된 패킷이 들어오면 daemon이 실행
```

### 5) Recovery (Serial Console)
```bash
# 예: Linux
sudo minicom -D /dev/ttyACM0 -b 115200
```

---

## 이 프로젝트가 “강한” 이유

- **현장 친화적**: 네트워크/SSH가 불안정해도 “동글 + 시리얼”로 복구 가능
- **단순한 인터페이스**: Qt/daemon은 그냥 `/dev/*` 파일 I/O
- **고정 프레임(256Byte)**: 패킷 경계/검증이 쉬워서 디버깅이 편함
- **재현성**: PC에서 적재한 패킷을 그대로 저장/전송하니 데모/테스트가 안정적

---

## 레포 구조 (예시)

```text
USB-MACRO/
├─ firmware/          # STM32(Black Pill) 펌웨어 (TinyUSB)
├─ kernel_driver/     # Linux USB vendor char driver (/dev/custom_usb_*)
├─ pc_client_qt/      # Qt GUI (명령 생성/적재)
├─ rpi_daemon/        # 명령 파싱/실행 데몬
└─ docs/              # 상세 설계/프로토콜 문서
```

---

## 더 자세한 설명(상세/기술 문서)

메인 README는 “한 번에 이해”가 목표라서 일부 세부를 뺐습니다.  
**프로토콜/SD 메타(info_struct)/펌웨어 동작 등 깊게 보려면** 아래 문서를 참고하세요.

- `docs/README_TECHNICAL.md` (상세 설계 문서로 이동 추천)
- 설계 고민 기록: https://github.com/won-jong-wan/USB-Macro/discussions/38

---

## License
MIT
