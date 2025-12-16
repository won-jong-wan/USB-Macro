/*
 * vendor.c
 *
 *  Created on: Dec 16, 2025
 *      Author: jonwo
 */
#include <usb_class.h>

// Vendor 데이터 수신 콜백
void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint32_t bufsize)
{
  (void) itf;    // 사용 안 함
  (void) buffer; // [중요] 매개변수 무시! (믿지 않음)
  (void) bufsize;// [중요] 매개변수 무시! (믿지 않음)

  // 1. TinyUSB 내부에 진짜 쌓여있는 데이터 양을 확인
  uint32_t available = tud_vendor_available();

  // 2. 데이터가 진짜 있다면?
  if ( available > 0 )
  {
    uint8_t my_buffer[64]; // 우리가 직접 마련한 그릇

    // 3. 수동으로 읽어오기 (여기서 FIFO를 비웁니다)
    // 읽어온 바이트 수를 리턴받음
    uint32_t count = tud_vendor_read(my_buffer, sizeof(my_buffer));

    // 읽어온 만큼만 처리
    if (count > 0)
    {
       // [디버깅] LED 토글 (데이터 진짜 읽음!)
       HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

       // 4. Echo Back (받은 거 그대로 돌려주기)
       tud_vendor_write(my_buffer, count);

       // 5. [필수] 즉시 전송 명령 (Flush)
       tud_vendor_write_flush();
    }
  }
}

void vendor_task(void)
{
  static uint32_t start_ms = 0;

  // 1초마다 실행
  if (HAL_GetTick() - start_ms < 1000) return;
  start_ms += 1000;

  if (tud_mounted())
  {
    char *msg = "1234\n";
    // 1초마다 "Alive"라는 메시지를 강제로 큐에 넣음
    tud_vendor_write(msg, 5);

    // 강제로 플러시(Flush) 시도 - 버퍼에 있는 걸 즉시 하드웨어로 밀어내기
    tud_vendor_write_flush();
  }
}
