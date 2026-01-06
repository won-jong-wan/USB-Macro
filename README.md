# USB-MACRO 

 

<p align="center">
  <img src="https://github.com/user-attachments/assets/073ce760-7c3b-4340-8ca4-f8242566badd" width="100%">
</p>





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
[![작동 영상](https://img.youtube.com/vi/8KH_-n1Bi2A/hqdefault.jpg)](https://youtu.be/8KH_-n1Bi2A?si=F_2Mj6nK8ccp6BNA)

- PC에서 버튼 클릭(명령어 적재)
- **Black Pill을 로봇에 꽂기**
- (트리거 한 번) **자동 실행**
- 네트워크가 죽어도 **시리얼 콘솔로 복구**
---
## H/W BOM
<p align="center">
  <img src="https://github.com/user-attachments/assets/00319191-aee3-46ba-9975-b15120f0ba38" width="100%">
</p>
 
---
<div align="center">

<table>
  <thead>
    <tr>
      <th><big><b>부품명</b></big></th>
      <th><big><b>역할</b></big></th>
    </tr>
  </thead>
  <tbody>
    <tr><td><big>로터리 엔코더</big></td><td><big>사용자 입력 장치</big></td></tr>
    <tr><td><big>SSD1306 OLED</big></td><td><big>시스템 상태 출력 디스플레이</big></td></tr>
    <tr><td><big>HAM1927 (SD카드 모듈)</big></td><td><big>외부 저장 장치</big></td></tr>
    <tr><td><big>Black Pill (STM32F411)</big></td><td><big>MCU</big></td></tr>
    <tr><td><big>라즈베리파이 디버그 프로브</big></td><td><big>펌웨어 업데이트 및 추가적인 CDC 지원</big></td></tr>
    <tr><td><big>터틀봇3</big></td><td><big>최종 타겟 호스트(Host)</big></td></tr>
  </tbody>
</table>

</div>


---


## 2가지 모드

### ✅ MAIN: 자동 실행 (Vendor 256Byte)
- PC: `/dev/custom_usb_pc` 로 256Byte 패킷 write
- Dongle: 수신한 패킷을 SD에 저장
- Robot: `/dev/custom_usb_rpi` 로 256Byte 패킷 read → daemon이 S/D/C로 파싱/실행

<img width="2890" height="815" alt="image" src="https://github.com/user-attachments/assets/2170902d-aa80-486e-83f7-61e9843547cd" />


### 🆘 EMERGENCY: 네트워크 죽어도 복구 (CDC↔UART Serial Console)
- Black Pill이 **CDC 모드**로 전환
- PC는 **가상 COM 포트**로 접속
- CDC 데이터가 UART로 브릿지되어 RPi의 **agetty 시리얼 콘솔**로 연결

<img width="2592" height="316" alt="image" src="https://github.com/user-attachments/assets/36be97b2-150e-45ed-81db-9d8bc961556f" />


---

## Quick Start (요약)


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





















