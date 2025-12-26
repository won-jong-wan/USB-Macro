#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

// --- 하드웨어 설정 ---
#if defined(STM32F4)
  #define CFG_TUSB_MCU    OPT_MCU_STM32F4
#elif defined(STM32F1)
  #define CFG_TUSB_MCU    OPT_MCU_STM32F1
#else
#define CFG_TUSB_MCU    OPT_MCU_STM32F4
#endif

#define CFG_TUSB_RHPORT0_MODE       (OPT_MODE_DEVICE)
#define CFG_TUSB_OS                 OPT_OS_NONE
#define CFG_TUD_ENDPOINT0_SIZE      64

// --- 기능 활성화 (전부 1로 둡니다) ---
#define CFG_TUD_CDC               1
#define CFG_TUD_MSC               1
#define CFG_TUD_HID               1
#define CFG_TUD_VENDOR            1

// --- 버퍼 설정 ---
#define CFG_TUD_MSC_EP_BUFSIZE    4096
#define CFG_TUD_HID_EP_BUFSIZE    16
#define CFG_TUD_VENDOR_RX_BUFSIZE 0
#define CFG_TUD_VENDOR_TX_BUFSIZE 512

// [추가] CDC 버퍼 설정 (이게 없으면 컴파일 에러 남)
#define CFG_TUD_CDC_RX_BUFSIZE    64
#define CFG_TUD_CDC_TX_BUFSIZE    64

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */
