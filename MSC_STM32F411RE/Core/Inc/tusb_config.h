#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

// 1. 하드웨어 설정 (본인 칩에 맞게 수정 필요!)
// STM32F4 등을 쓴다면 보통 OPT_MODE_DEVICE | OPT_MCU_STM32F4 라고 적습니다.
// 일단 자동으로 잡도록 시도해봅니다.
#if defined(STM32F4)
  #define CFG_TUSB_MCU    OPT_MCU_STM32F4
#elif defined(STM32F1)
  #define CFG_TUSB_MCU    OPT_MCU_STM32F1
#else
  // 컴파일 에러가 나면 수동으로 지정해야 합니다.
  #define CFG_TUSB_MCU    OPT_MCU_STM32F4
#endif

#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_DEVICE)

// 2. 운영체제 사용 여부 (None)
#define CFG_TUSB_OS                 OPT_OS_NONE

// 3. 메모리 최적화
#define CFG_TUD_ENDPOINT0_SIZE    64

// 4. 기능 활성화 (MSC 켜기!)
#define CFG_TUD_CDC               0   // 시리얼 끄기
#define CFG_TUD_MSC               1   // 저장장치 켜기
#define CFG_TUD_HID               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0

// 5. MSC 버퍼 설정
#define CFG_TUD_MSC_EP_BUFSIZE    512

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */
