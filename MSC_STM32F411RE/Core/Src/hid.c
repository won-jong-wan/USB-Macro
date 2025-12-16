/*
 * hid.c
 *
 *  Created on: Dec 16, 2025
 *      Author: jonwo
 */

#include <usb_class.h>

// 1. Set Report Callback
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance; (void) report_id; (void) report_type; (void) buffer; (void) bufsize;
}

// 2. Get Report Callback
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  (void) instance; (void) report_id; (void) report_type; (void) buffer; (void) reqlen;
  return 0;
}
void hid_task(void)
{
  // 1. 10ms마다 실행 (폴링 간격 준수)
  // HAL_GetTick()을 사용하여 넌블로킹 딜레이 구현
  static uint32_t start_ms = 0;
  if (HAL_GetTick() - start_ms < 10) return;
  start_ms += 10;

  // 2. USB가 연결되어 있고, HID가 준비되었는지 확인
  // (중요: 준비 안 됐는데 보내면 다운됨)
  if ( !tud_mounted() || !tud_hid_ready() ) return;

  // ---------------------------------------------------------
  // [시나리오] 5초마다 "hello" 타이핑하고 엔터 치기
  // ---------------------------------------------------------
  static uint32_t send_ms = 0;
  static bool is_typing = false;

  // 5초 주기 체크
  if (HAL_GetTick() - send_ms > 5000)
  {
    send_ms = HAL_GetTick();
    is_typing = true; // 타이핑 시작 신호
  }

  if (is_typing)
  {
    // 보낼 키 시퀀스: h, e, l, l, o, ENTER
    // (USB HID 키코드: a=4, b=5 ... h=11 ...)
    static uint8_t const key_seq[] = {
      HID_KEY_H, HID_KEY_E, HID_KEY_L, HID_KEY_L, HID_KEY_O, HID_KEY_ENTER
    };
    static uint8_t seq_idx = 0;
    static bool key_pressed = false;

    if (key_pressed)
    {
      // [B] 키 떼기 (Release)
      // 키를 눌렀으면 반드시 "아무것도 안 누름(NULL)"을 보내야 함
      tud_hid_keyboard_report(0, 0, NULL);
      key_pressed = false;

      seq_idx++; // 다음 글자로 이동
      if (seq_idx >= sizeof(key_seq)) {
        seq_idx = 0;
        is_typing = false; // 타이핑 끝
      }
    }
    else
    {
      // [A] 키 누르기 (Press)
      uint8_t keycode[6] = { 0 };
      keycode[0] = key_seq[seq_idx];

      // 리포트 전송 함수 (Interface 번호, Modifier, Keycode배열)
      tud_hid_keyboard_report(0, 0, keycode);
      key_pressed = true;
    }
  }
}
