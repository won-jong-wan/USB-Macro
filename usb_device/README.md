# USB-Macro (STM32F411CEU6 기반)

## 프로젝트 개요
이 프로젝트는 STM32F411CEU6(블랙필 보드) 기반의 USB 매크로 장치로, SD카드를 활용한 복합 저장장치(MSC)와 Vendor 통신, 그리고 긴급 제어를 위한 CDC 모드를 지원합니다. OLED 디스플레이와 로터리 엔코더+스위치를 통해 모드 전환 및 명령 실행이 가능합니다.

---

## 주요 기능
- **MSC + Vendor 통신 모드**
  - SD카드의 절반은 MSC(대용량 저장장치), 절반은 Vendor(명령 저장/전송) 용도로 분할 사용
  - Vendor를 통해 명령어 구조체를 수신하여 SD카드에 저장
  - 로터리 엔코더+스위치로 write 명령을 내리면 저장된 명령을 Vendor로 전송
  - MSC 파일 시스템 손상 방지를 위해 영역 분리 및 동기적 SD카드 접근(DMA 사용)
- **CDC(긴급 모드)**
  - 긴급 상황에서 CDC(가상 COM포트)로 전환하여 별도 제어 가능
- **OLED 디스플레이**
  - 현재 모드 및 상태 실시간 표시
- **로터리 엔코더 + 스위치**
  - 모드 전환 및 명령 실행 제어

---

## 폴더 구조 및 설명
- `STM32F411CEU6_USB_Macro/` : 메인 펌웨어 소스 및 헤더, 드라이버, 라이브러리, 디버그 파일 등 포함
- `MSC_STM32F411RE/` : MSC 및 Vendor 통신의 선행 테스트/프로토타입 구현
- `sandbox/` : Windows 환경에서 Vendor 통신을 테스트하는 Python 스크립트 모음 ([자세한 설명](https://github.com/won-jong-wan/USB-Macro/discussions/41))
- `assets/images/` : 하드웨어 및 회로 관련 사진 자료 (예: 조립 사진, 배선 등)

---

## 하드웨어 구성
- **MCU**: STM32F411CEU6 (블랙필 보드)
- **저장장치**: microSD 카드
- **입력장치**: 로터리 엔코더 + 스위치
- **디스플레이**: OLED
- **USB**: Full-Speed 지원
- **회로도**: (추후 추가 예정, `assets/images/` 폴더 참고)

---

## 빌드 및 실행 방법
- STM32CubeIDE 등 STM32 시리즈를 지원하는 IDE에서 프로젝트 폴더(`STM32F411CEU6_USB_Macro/`)를 불러와 빌드 및 플래시
- 별도의 makefile도 포함되어 있으나, IDE 사용을 권장

---

## 참고 자료
- [USB-Macro Discussions #41 (Vendor 통신 테스트 스크립트 설명)](https://github.com/won-jong-wan/USB-Macro/discussions/41)
- TinyUSB, STM32 HAL 등 오픈소스 라이브러리 활용

---

## 기타
- 본 프로젝트는 포트폴리오 및 개인 연구용으로, 별도의 기여/라이선스 정책은 적용하지 않습니다.

---

## 하드웨어 사진 예시

![조립 사진1](../assets/images/DSC04957.JPG)
![조립 사진2](../assets/images/DSC04958.JPG)

(추가 사진은 `assets/images/` 폴더 참고)