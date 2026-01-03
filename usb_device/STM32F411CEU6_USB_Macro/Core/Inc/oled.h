#ifndef OLED_H
#define OLED_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#ifndef OLED_COL_OFFSET
#define OLED_COL_OFFSET 0   // 화면이 좌우로 밀리면 2로 바꿔보기
#endif

// SSD1306 SPI(4-wire) 기본 드라이버 (CS/DC/RST는 GPIO)
// 내부 버퍼에 그린 뒤 OLED_Update()로 실제 표시
void OLED_Init(SPI_HandleTypeDef *hspi);
void OLED_Clear(void);
void OLED_Update(void);

// 작은 글씨(5x7) : row=0~7, col_char=0~21
void OLED_DrawStr(uint8_t row, uint8_t col_char, const char *s);

// 큰 글씨(2배 확대) : x,y는 픽셀 좌표
void OLED_DrawStr2x(int x, int y, const char *s);

// 디버그용
void OLED_Fill(uint8_t pattern);

#endif
