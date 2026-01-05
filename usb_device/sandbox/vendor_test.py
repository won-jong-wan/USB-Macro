import usb.core
import usb.util
import struct
import sys
import time

VID = 0xCAFE 
PID = 0x4000
PACKET_SIZE = 256
STRUCT_FMT = '<IBH249s' 

def main():
    print("📡 라즈베리파이 수신 모드 (STM32 -> Pi -> STM32)")
    
    dev = usb.core.find(idVendor=VID, idProduct=PID)
    if dev is None:
        print("❌ 장치 없음")
        return

    # 설정 및 드라이버 분리
    cfg = dev.get_active_configuration()
    intf = next((i for i in cfg if i.bInterfaceClass == 0xFF), None)
    # if dev.is_kernel_driver_active(intf.bInterfaceNumber):
    #     dev.detach_kernel_driver(intf.bInterfaceNumber)

    ep_out = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
    ep_in = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)

    print("👂 데이터 대기 중... (Ctrl+C로 종료)")

    while True:
        try:
            # 1. STM32로부터 데이터 읽기 (Timeout을 넉넉히 2초 줌)
            # STM32가 1초마다 보내므로 2초면 충분히 잡힘
            data = dev.read(ep_in, PACKET_SIZE, timeout=2000)
            
            if len(data) == PACKET_SIZE:
                magic, info, length, cmd = struct.unpack(STRUCT_FMT, data)
                msg = cmd[:length].decode('utf-8', errors='ignore')
                
                print(f"📥 [수신] \"{msg}\" (Info: 0x{info:02X})")

                # 2. 데이터를 받자마자 답장 보내기 ("LED_ON")
                reply_str = "LED_ON"
                reply_bytes = reply_str.encode('utf-8')
                
                # 구조체 싸서 보내기
                payload = struct.pack(STRUCT_FMT, 0x99999999, 0x11, len(reply_bytes), reply_bytes)
                dev.write(ep_out, payload)
                print(f"📤 [답장] \"{reply_str}\" 전송 완료 -> STM32 LED 켜짐")
                print("-" * 40)

        except usb.core.USBError as e:
            # 타임아웃은 에러가 아님 (그냥 데이터가 안 온 것)
            if e.errno == 110: 
                continue
            print(f"에러: {e}")
        except KeyboardInterrupt:
            break

if __name__ == "__main__":
    main()