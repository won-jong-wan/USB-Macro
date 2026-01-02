#ifndef UI_H
#define UI_H

#include <stdint.h>

typedef enum {
  UI_STATE_ROOT_MENU = 0,
  UI_STATE_LINUX_MODE,
  UI_STATE_EMERGENCY_MODE
} UI_State;

extern UI_State g_s_state;

void UI_Init(void);
UI_State UI_GetState(void);
void UI_Process(int step, uint8_t key_short);

#endif // UI_H
