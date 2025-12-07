#include "tusb.h"

// 1. Device Descriptor
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0xCAFE, // 임의의 ID
    .idProduct          = 0x4000, // 임의의 ID
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void) { return (uint8_t const *) &desc_device; }

// 2. Configuration Descriptor
enum { ITF_NUM_MSC, ITF_NUM_TOTAL };

#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN)

// Endpoint 번호 (STM32는 보통 0x81, 0x01, 0x82, 0x02 등을 씁니다. 충돌나면 바꿔야 함)
#define EPNUM_MSC_OUT     0x01
#define EPNUM_MSC_IN      0x81

uint8_t const desc_configuration[] =
{
  // Config
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

  // MSC Interface
  TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 0, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
  (void) index;
  return desc_configuration;
}

// 3. String Descriptor (생략하면 코드가 너무 길어지니 일단 NULL 처리하거나 기본만)
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) index; (void) langid;
  return NULL; // 문자열 없어도 동작은 합니다.
}
