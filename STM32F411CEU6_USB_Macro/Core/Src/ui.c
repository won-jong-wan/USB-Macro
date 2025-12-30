#include "ui.h"
#include "oled.h"

#include <stdbool.h>

/* ===== 내부 상태 ===== */
enum {
    USB_MODE_CDC = 0,       // 긴급/디버그 모드 (CDC Only)
    USB_MODE_MSC_VENDOR = 1,    // 펌웨어 업데이트 등 (MSC + Vendor)
    USB_MODE_HID_MSC = 2,        // 평상시 사용 (HID + MSC)
};

extern volatile bool usb_ready;
extern void ven_send();
extern uint8_t g_usb_mode;

UI_State g_s_state = UI_STATE_ROOT_MENU;
static uint8_t s_root_sel = 0;   // 0=Linux, 1=Emergency

static void UpperTag(void) {
	switch (g_usb_mode) {
	case USB_MODE_MSC_VENDOR:
		OLED_DrawStr(0, 0, "Linux Mode");
		break;
	case USB_MODE_CDC:
		OLED_DrawStr(0, 0, "Emergency Mode");
		break;
	default:
		OLED_DrawStr(0, 0, "Loading...");
		break;
	}
}

/* ===== 화면 출력 ===== */
static void DrawRootMenu(void) {
	OLED_Clear();
	UpperTag();

	int y0 = 20, y1 = 40;

	if (s_root_sel == 0)
		OLED_DrawStr2x(0, y0, ">");
	OLED_DrawStr2x(14, y0, "Linux");

	if (s_root_sel == 1)
		OLED_DrawStr2x(0, y1, ">");
	OLED_DrawStr2x(14, y1, "Emergency");

	OLED_Update();
}

static void DrawLinuxMode(void) {
	OLED_Clear();
	UpperTag();

	int y0 = 20, y1 = 40;

	if (s_root_sel == 0)
		OLED_DrawStr2x(0, y0, ">");
	OLED_DrawStr2x(14, y0, "Write");

	if (s_root_sel == 1)
		OLED_DrawStr2x(0, y1, ">");
	OLED_DrawStr2x(14, y1, "No Write");

	OLED_DrawStr(7, 0, "SELECT: back to menu");
	OLED_Update();
}

static void DrawEmergencyMode(void) {
	OLED_Clear();
	UpperTag();

	OLED_DrawStr2x(0, 20, "Emergency");
	OLED_DrawStr2x(0, 40, "Active");
	OLED_DrawStr(7, 0, "KEY: back to menu");
	OLED_Update();
}

/* ===== API ===== */

void UI_Init(void) {
	g_s_state = UI_STATE_ROOT_MENU;
	s_root_sel = 0;
	DrawRootMenu();
}

UI_State UI_GetState(void) {
	return g_s_state;
}

void UI_Process(int step, uint8_t key_short) {
	switch (g_s_state) {
	/* ===== ROOT MENU ===== */
	case UI_STATE_ROOT_MENU:
		DrawRootMenu();
		if (step != 0) {
			s_root_sel ^= 1;  // 0 <-> 1 토글
			DrawRootMenu();
		}
		if (key_short) {
			if (s_root_sel == 0) {
				g_s_state = UI_STATE_LINUX_MODE;
				if (g_usb_mode != USB_MODE_MSC_VENDOR) {
					usb_ready = false;
				}
				DrawLinuxMode();
			} else {
				g_s_state = UI_STATE_EMERGENCY_MODE;
				if (g_usb_mode != USB_MODE_CDC) {
					usb_ready = false;
				}
				DrawEmergencyMode();
			}
		}
		break;

		/* ===== LINUX MODE ===== */
	case UI_STATE_LINUX_MODE:
		DrawLinuxMode();
		if (step != 0) {
			s_root_sel ^= 1;  // 0 <-> 1 토글
			DrawLinuxMode();
		}
		if (key_short) {
			if (s_root_sel == 0) {
				ven_send();
			}
			g_s_state = UI_STATE_ROOT_MENU;
			usb_ready = false;
			DrawRootMenu();
		}
		// 여기에 세부 메뉴 로직을 추가하면 됩니다
		break;

		/* ===== EMERGENCY MODE ===== */
	case UI_STATE_EMERGENCY_MODE:
		DrawEmergencyMode();
		if (key_short) {
			g_s_state = UI_STATE_ROOT_MENU;
			usb_ready = false;
			DrawRootMenu();
		}
		// 여기에 세부 메뉴 로직을 추가하면 됩니다
		break;

	default:
		g_s_state = UI_STATE_ROOT_MENU;
		usb_ready = false;
		DrawRootMenu();
		break;
	}
}
