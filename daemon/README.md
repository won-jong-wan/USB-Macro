# User Application

### 🎯 프로젝트 개요
- STM32 마이크로컨트롤러와 라즈베리파이 간 자작 커널 드라이버 기반 USB 통신을 통해 명령어 자동 실행 시스템에서 명령어를 실행하는 유저 애플리케이션입니다.


## ⚡일반 모드
![Vendor](./usb_webp/vendor_ex.webp)

<img width="1166" height="1301" alt="image" src="https://github.com/user-attachments/assets/291ae0ee-1c81-4be4-8830-568278f8cc80" />


### ✨ 주요 기능
* 1️⃣ USB 자동 감지 및 연결
  *  **Udev 기반 모니터링** : USB 모듈 장치 연결 및 해제를 실시간으로 감지
  *  **자동 재연결** : 장치가 제거되었다가 다시 연결되면 자동으로 복구
  *  **Poll 기반 멀티 플랙싱** : Udev 이벤트와 USB 데이터 수신을 동시에 감


* 2️⃣ 명령어 분류 및 실행
  * STM32에서 전송된 패킷의 INFO 필드를 분석하여 명령어 타입을 구분하여 실행
    | 타입 | 설명 | 실행 방식 |
    |-------|-------|-------|
    | S-Node | 실행 완료 시점이 명확한 명령어(예 : ls, pwd ,cd) | 지속적인 Bash 세션에서 순차 실행을 통해 명령어가 끝날 때까지 대기 |
    | C-Node | 실행 완료 시점이 불명확한 명령어(예 : ping 등) | 자식 프로세스를 생성하여 백그라운드에서 실행 |
    | D-Node (딜레이) | 명령어 실행 후 안정화 대기 | 지정된 시간(초) 동안 대기 후 다음 명령어 실행 |
   * #### 🔄 환경변수 동기화 (S-Node → C-Node)
     * S-Node에서 설정한 환경변수가 C-Node에도 자동 전달

    
* 3️⃣ 멀티 프로세스 구조 (User/Root)
  * 프로그램 실행 시 두 개의 프로세스를 생성하면서 시작
  * 명령어에 sudo(예 : sudo apt update, sudo apt upgrade)가 있으면 Root(#)프로세스에서 명령어 실행
  * sudo 를 지워주면서(sudo apt update -> apt update) 비밀번호를 입력하지 않고 명령어 실행
  * 한쪽 Shell에서 디렉토리를 이동하면, **다른 Shell도 자동으로 같은 경로로 이동**



## 🆘 CDC 모드

![CDC UART](./usb_webp/cdc_uart_ex.webp)


### 📌 CDC 모드란?
**CDC (Communications Device Class)** 모드는 네트워크가 단절된 긴급 상황에서 STM32-Raspberry Pi 간 USB를 통해 **직접 터미널에 접속**할 수 있는 백업 시스템입니다.
 
### 🎯 사용 목적

| 상황 | 설명 |
|------|------|
| **네트워크 단절** | SSH 접속 불가 시 USB로 직접 접속 |
| **긴급 복구** | 시스템 오류로 원격 접속이 안 될 때 |
| **디버깅** | 일반 모드 드라이버 문제 발생 시 |
| **네트워크 없는 환경** | 초기 설정이나 보안 환경에서 사용 |


### 🔄 일반 모드 vs CDC 모드

| 모드 | 통신 경로 | 동작 방식 |
|------|----------|----------|
| **일반 모드**<br>(Primary) | STM32 → Custom Driver → /dev/team_own_stm32 → usb_app | 자동 명령어 실행 |
| **CDC 모드**<br>(Emergency) | STM32 → USB CDC ACM → /dev/ttyACM0 → agetty → Shell | 수동 터미널 접속 |

### 아쉬운 점
* htop, ls 등 다양한 명령어는 가능했지만, Vi, Nano 같이 코드 작성할 수 있는 편집기들을 사용한 것에서는 @@@와 같은 쓰레기 문자가 생기거나, I(Insert)를 해도 입력이 제대로 안되는 아쉬움이 있었음 

### agetty (Alternative Getty)
**agetty**는 Linux 시스템에서 **시리얼 포트(Serial Port)에 로그인 프롬프트를 제공**하는 프로그램입니다.

"Getty"는 "Get Teletype"의 약자로, 전통적으로 터미널 장치에 로그인 화면을 띄워주는 역할을 합니다.

### **1️⃣ 일반 로그인 vs 시리얼 로그인**
[일반 로그인 (SSH)]
키보드/네트워크 → Linux → 로그인 프롬프트

[시리얼 로그인 (agetty)]
USB CDC → /dev/ttyACM0 → agetty → 로그인 프롬프트
