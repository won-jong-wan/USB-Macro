#include "tusb.h"

// --------------------------------------------------------------------+
// 0. 모드 정의 및 전역 변수
// --------------------------------------------------------------------+

// [중요] main.h 등에 이 enum을 정의해서 공유하면 좋습니다.
enum {
    USB_MODE_CDC = 0,       // 긴급/디버그 모드 (CDC Only)
    USB_MODE_MSC_VENDOR = 1,    // 펌웨어 업데이트 등 (MSC + Vendor)
    USB_MODE_HID_MSC = 2,        // 평상시 사용 (HID + MSC)
};

// 기존 bool 대신 uint8_t로 변경하여 3가지 상태를 표현
extern volatile uint8_t g_usb_mode;

// --------------------------------------------------------------------+
// 1. Device Descriptors
// --------------------------------------------------------------------+

// 모드별 PID 정의 (충돌 방지용)
#define PID_CDC         0xDEAD
#define PID_MSC_VEN     0x4000
#define PID_HID_MSC     0x4001

// 1) CDC 모드 (Class 0x02)
tusb_desc_device_t const desc_device_cdc = {
    .bLength = 18, .bDescriptorType = TUSB_DESC_DEVICE, .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_CDC, .bDeviceSubClass = 0, .bDeviceProtocol = 0,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0xCAFE, .idProduct = PID_CDC, .bcdDevice = 0x0100,
    .iManufacturer = 1, .iProduct = 2, .iSerialNumber = 3, .bNumConfigurations = 1
};

// 2) MSC + Vendor 모드 (Composite 0x00)
tusb_desc_device_t const desc_device_msc_ven = {
    .bLength = 18, .bDescriptorType = TUSB_DESC_DEVICE, .bcdUSB = 0x0200,
    .bDeviceClass = 0x00, .bDeviceSubClass = 0, .bDeviceProtocol = 0,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0xCAFE, .idProduct = PID_MSC_VEN, .bcdDevice = 0x0100,
    .iManufacturer = 1, .iProduct = 2, .iSerialNumber = 3, .bNumConfigurations = 1
};

// 3) HID + MSC 모드 (Composite 0x00)
tusb_desc_device_t const desc_device_hid_msc = {
    .bLength = 18, .bDescriptorType = TUSB_DESC_DEVICE, .bcdUSB = 0x0200,
    .bDeviceClass = 0x00, .bDeviceSubClass = 0, .bDeviceProtocol = 0,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0xCAFE, .idProduct = PID_HID_MSC, .bcdDevice = 0x0100,
    .iManufacturer = 1, .iProduct = 2, .iSerialNumber = 3, .bNumConfigurations = 1
};

// [콜백] 현재 모드에 맞는 Device Descriptor 반환
uint8_t const * tud_descriptor_device_cb(void) {
    switch (g_usb_mode) {
        case USB_MODE_CDC:        return (uint8_t const *)&desc_device_cdc;
        case USB_MODE_MSC_VENDOR: return (uint8_t const *)&desc_device_msc_ven;
        case USB_MODE_HID_MSC:    return (uint8_t const *)&desc_device_hid_msc;
        default:                  return (uint8_t const *)&desc_device_cdc; // 안전장치
    }
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
// 3. Configuration Descriptors
// --------------------------------------------------------------------+

// Endpoint 번호 정의
// 모드가 바뀌면 USB가 리셋되므로 EP 번호를 재사용해도 됩니다.
#define EPNUM_MSC_OUT     0x01
#define EPNUM_MSC_IN      0x81
#define EPNUM_VENDOR_OUT  0x02
#define EPNUM_VENDOR_IN   0x82
#define EPNUM_HID         0x83 // IN only

#define EPNUM_CDC_NOTIF   0x81
#define EPNUM_CDC_OUT     0x02
#define EPNUM_CDC_IN      0x82

// --- [Config A] CDC Only ---
#define CONFIG_CDC_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)

uint8_t const desc_configuration_cdc[] = {
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, CONFIG_CDC_LEN, 0x00, 100),
    TUD_CDC_DESCRIPTOR(0, 0, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64)
};

// --- [Config B] MSC + Vendor ---
#define CONFIG_MSC_VEN_LEN (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN + TUD_VENDOR_DESC_LEN)

uint8_t const desc_configuration_msc_ven[] = {
    // Interface Count = 2
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, CONFIG_MSC_VEN_LEN, 0x00, 100),

    // Interface 0: MSC
    TUD_MSC_DESCRIPTOR(0, 0, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),

    // Interface 1: Vendor
    TUD_VENDOR_DESCRIPTOR(1, 0, EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN, 64)
};

// --- [Config C] HID + MSC ---
#define CONFIG_HID_MSC_LEN (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN + TUD_HID_DESC_LEN)

uint8_t const desc_configuration_hid_msc[] = {
    // Interface Count = 2
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, CONFIG_HID_MSC_LEN, 0x00, 100),

    // Interface 0: MSC
    TUD_MSC_DESCRIPTOR(0, 0, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),

    // Interface 1: HID
    TUD_HID_DESCRIPTOR(1, 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(desc_hid_report), EPNUM_HID, 16, 10)
};


// [콜백] 현재 모드에 맞는 Configuration Descriptor 반환
uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
    (void) index;
    switch (g_usb_mode) {
        case USB_MODE_CDC:        return desc_configuration_cdc;
        case USB_MODE_MSC_VENDOR: return desc_configuration_msc_ven;
        case USB_MODE_HID_MSC:    return desc_configuration_hid_msc;
        default:                  return desc_configuration_cdc;
    }
}

// --------------------------------------------------------------------+
// 4. String Descriptor
// --------------------------------------------------------------------+
char const* string_desc_arr [] = {
    (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
    "TinyUSB",                     // 1: Manufacturer
    "USB-Macro",           // 2: Product
    "123456",                      // 3: Serials
};

static uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;
    uint8_t chr_count;

    if ( index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

        const char* str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str);
        if ( chr_count > 31 ) chr_count = 31;

        for(uint8_t i=0; i<chr_count; i++) {
            _desc_str[1+i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

    return _desc_str;
}
