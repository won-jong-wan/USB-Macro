#include "tusb.h"

// --------------------------------------------------------------------+
// 1. Device Descriptor (Class는 0x00으로 둡니다)
// --------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00, // 각 인터페이스에서 정의함 (Composite Device)
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0xCAFE,
    .idProduct          = 0x4000,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void) { return (uint8_t const *) &desc_device; }

// --------------------------------------------------------------------+
// 2. HID Report Descriptor (키보드 지도)
// --------------------------------------------------------------------+
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// --------------------------------------------------------------------+
// 3. Configuration Descriptor (3단 합체!)
// --------------------------------------------------------------------+

// A. 인터페이스 번호 순서 (0:MSC -> 1:HID -> 2:Vendor)
enum {
  ITF_NUM_MSC,
  ITF_NUM_HID,
  ITF_NUM_VENDOR,
  ITF_NUM_TOTAL
};

// B. 전체 길이 계산 (세 덩어리의 합)
#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN + TUD_HID_DESC_LEN + TUD_VENDOR_DESC_LEN)

// C. 엔드포인트(EP) 주소 배정 [중요!]
// STM32F4는 EP가 넉넉하므로 순서대로 1, 2, 3번을 줍니다.

// MSC (Interface 0) : EP 1번 사용
#define EPNUM_MSC_OUT     0x01
#define EPNUM_MSC_IN      0x81

// HID (Interface 1) : EP 2번 사용
#define EPNUM_HID         0x83

// VENDOR (Interface 2) : EP 3번 사용
#define EPNUM_VENDOR_OUT  0x02
#define EPNUM_VENDOR_IN   0x82

uint8_t const desc_configuration[] =
{
  // Config Header
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

  // ------------------------------------------------------------------
  // Interface 0: MSC (Mass Storage)
  // ------------------------------------------------------------------
  TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),

  // ------------------------------------------------------------------
  // Interface 1: HID (Keyboard)
  // ------------------------------------------------------------------
  TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report), EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 10),

  // ------------------------------------------------------------------
  // Interface 2: Vendor (Linux Custom Driver)
  // ------------------------------------------------------------------
  // 매크로 인자: Interface#, String Index, EP Out, EP In, EP Size
  // Vendor Class는 자동으로 0xFF로 설정됩니다.
  TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 0, EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN, 64)
};

// Configuration 요청 콜백
uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
  (void) index;
  return desc_configuration;
}

// HID Report 요청 콜백
uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance) {
  (void) instance;
  return desc_hid_report;
}

// 문자열 요청 콜백 (기본값)
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) index; (void) langid;
  return NULL;
}
