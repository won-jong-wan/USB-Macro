#include "tusb.h"

// main.c에서 제어할 전역 변수
extern bool g_is_emergency_mode;

// [추가 1] IAD 길이 정의 (이게 없어서 에러가 났습니다)
#define TUD_IAD_DESC_LEN    9

// [추가 2] IAD 디스크립터 정의 (아까 추가한 것)
#define TUD_IAD_DESCRIPTOR(_first_itf, _str_idx, _itf_count, _class, _subclass, _protocol) \
  9, TUSB_DESC_INTERFACE_ASSOCIATION, _first_itf, _itf_count, _class, _subclass, _protocol, _str_idx

// --------------------------------------------------------------------+
// 1. Device Descriptors (두 가지 버전)
// --------------------------------------------------------------------+

// [A] 평상시 모드 (기존 설정)
tusb_desc_device_t const desc_device_normal =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCAFE,
    .idProduct          = 0x4000, // 평상시 PID
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

// [B] 긴급 모드 (CDC 단독 장치)
// IAD를 쓰지 않고, Device Class 자체를 CDC(0x02)로 설정합니다.
// 이렇게 하면 윈도우가 훨씬 안정적으로 COM 포트를 잡습니다.
tusb_desc_device_t const desc_device_emergency =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

    // [핵심 변경 1] IAD(0xEF) 대신 CDC(0x02)를 직접 사용
    .bDeviceClass       = TUSB_CLASS_CDC,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,

    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCAFE,

    // [핵심 변경 2] PID를 무조건 바꿔야 합니다!
    // 같은 PID를 쓰면 윈도우가 이전 드라이버(복합장치)를 로딩하려다 충돌납니다.
    .idProduct          = 0xDEAD,

    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

// Device Descriptor 콜백
uint8_t const * tud_descriptor_device_cb(void) {
    return (uint8_t const *) (g_is_emergency_mode ? &desc_device_emergency : &desc_device_normal);
}

// --------------------------------------------------------------------+
// 2. HID Report Descriptor
// --------------------------------------------------------------------+
uint8_t const desc_hid_report[] = { TUD_HID_REPORT_DESC_KEYBOARD() };

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance) {
  (void) instance;
  return desc_hid_report;
}

// --------------------------------------------------------------------+
// 3. Configuration Descriptors (두 가지 버전)
// --------------------------------------------------------------------+

// ---- [A] 평상시 구성 (MSC + HID + Vendor) ----
#define CONFIG_NORMAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN + TUD_HID_DESC_LEN + TUD_VENDOR_DESC_LEN)

// EP 번호 (평상시)
#define EPNUM_MSC_OUT     0x01
#define EPNUM_MSC_IN      0x81
#define EPNUM_VENDOR_OUT  0x02
#define EPNUM_VENDOR_IN   0x82
#define EPNUM_HID         0x83

uint8_t const desc_configuration_normal[] =
{
  TUD_CONFIG_DESCRIPTOR(1, 3, 0, CONFIG_NORMAL_LEN, 0x00, 100),
  TUD_MSC_DESCRIPTOR(0, 0, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
  TUD_HID_DESCRIPTOR(1, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report), EPNUM_HID, 16, 10),
  TUD_VENDOR_DESCRIPTOR(2, 0, EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN, 64)
};


// ---- [B] 긴급 모드 구성 (CDC Only) ----
// IAD를 제거하여 구조를 단순화합니다.

// [핵심 변경 3] IAD 길이 제거
#define CONFIG_EMERGENCY_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)

// EP 번호: 1번과 2번 사용 (평상시와 겹쳐도 리셋되므로 상관없음)
#define EPNUM_CDC_NOTIF   0x81
#define EPNUM_CDC_OUT     0x02
#define EPNUM_CDC_IN      0x82

uint8_t const desc_configuration_emergency[] =
{
    // Interface Count = 2 (CDC Control + CDC Data)
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, CONFIG_EMERGENCY_LEN, 0x00, 100),

    // [핵심 변경 4] IAD 제거함! 필요 없음!

    // CDC Interface
    // 인자: Interface#, StringIdx, EP Notif, EP Notif Size, EP Out, EP In, EP Size
    TUD_CDC_DESCRIPTOR(0, 0, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64)
};


// Configuration Descriptor 콜백
uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
  (void) index;
  // 모드에 따라 다른 배열 반환
  return (g_is_emergency_mode ? desc_configuration_emergency : desc_configuration_normal);
}

// String Descriptor (간단하게 유지)
uint16_t const desc_string_langid[] = { 4, TUSB_DESC_STRING, 0x0409 };
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) langid;
  if (index == 0) return desc_string_langid;
  return NULL;
}
