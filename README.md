# USB-MACRO 

 

<p align="center">
  <img src="https://github.com/user-attachments/assets/073ce760-7c3b-4340-8ca4-f8242566badd" width="100%">
</p>





---

## 한 문장으로

> **PC에서 “명령 패킷들”만 미리 적재해두고 → 로봇에 꽂으면 → 로봇이 알아서 실행합니다.**  
> 메인 Daemon이 죽어도 **CDC↔UART 시리얼 콘솔**로 바로 복구합니다.

---

## 🔄 Before / After

### ❌ Before (현장 지옥)
- 로봇 AP/망 붙이기  
- SSH 접속 → ROS 환경 export  
- 터미널 여러 개 켜서 bringup / 센서 / 노드 실행  
- 네트워크 끊기면… 끝 (현장 멘붕)

### ✅ After (Plug & Run ☕)
- PC에서 버튼 클릭 (명령 적재)
- **Black Pill을 로봇에 꽂기**
- (트리거 1회) **자동 실행**
- 메인 Daemon 죽어도 **시리얼 콘솔로 복구 가능**


<details>
<summary><b>🎬 Demo Video (클릭하여 보기)</b></summary>

<p align="center">
  <a href="https://youtu.be/8KH_-n1Bi2A?si=F_2Mj6nK8ccp6BNA">
    <img src="https://img.youtube.com/vi/8KH_-n1Bi2A/hqdefault.jpg" width="420">
  </a>
</p>

</details>


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

## Quick Start 

### 1) PC Qt Application (Command STORE)
```bash
wget https://github.com/won-jong-wan/USB-Macro/releases/download/v1.0.0/USBMacroCommandStudio-x86_64.AppImage
chmod +x USBMacroCommandStudio-x86_64.AppImage
sudo ./USBMacroCommandStudio-x86_64.AppImage
```

### 2) Daemon Deployment
```bash
git clone git@github.com:won-jong-wan/USB-Macro.git
cd USB-Macro/daemon/
sudo ./install_services.sh

# 만약 .ko 파일이 호환되지 않는 경우
cd ../usb_macro_kernel
make
mv usb_macro.ko ../daemon
cd ../daemon
sudo ./install_services.sh
```

### 3) 펌웨어 업로드
```bash
cd usb_device/STM32F411CEU6_USB_Macro
# STM32CubeIDE 이용하여 업로드
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

## 📚 더 자세한 설명 (상세 / 기술 문서)

메인 README는 **한 번에 전체 구조를 이해하는 것**을 목표로 구성했습니다.  
구현 세부, 내부 동작, 설계 고민은 아래 문서를 참고하세요.
---
### 🔌 STM32 Device (Black Pill)
- CUSTOM USB Vendor Device 펌웨어
- PC로부터 패킷 수신 및 내부 저장 (256B frame)
- Robot 연결 시 패킷 전달 트리거 역할

[STM32 device(black pill)](https://github.com/won-jong-wan/USB-Macro/tree/main/usb_device)

---

### 🖥️ PC (Qt Application)
- 명령 생성 및 패킷 적재 (STORE)
- GUI 기반 커맨드 작성

[Qt Application (PC)](https://github.com/won-jong-wan/USB-Macro/tree/main/usb_macro_writerQt)

---

### 🐧 Kernel (Custom USB Driver)
- CUSTOM USB Vendor Device 제어
- PC / RPi 공용 커널 드라이버

[Kernel Driver](https://github.com/won-jong-wan/USB-Macro/tree/main/usb_macro_kernel)

---

### 🤖 Daemon (Robot Executor)
- Raspberry Pi 상주 데몬
- 저장된 명령 자동 실행 (RUN)

[Daemon](https://github.com/won-jong-wan/USB-Macro/tree/main/daemon)

---

### 🧠 Design Notes
- 구조 설계 및 트레이드오프 기록

[Discussion](https://github.com/won-jong-wan/USB-Macro/discussions/38)

---

## License
MIT















