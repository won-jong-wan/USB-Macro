import usb.core
import usb.util
import struct
import sys
import time

# ------------------------------------------------
# 1. 설정 (기존 코드와 동일하게 유지)
# ------------------------------------------------
VID = 0xCAFE 
PID = 0x4000
PACKET_SIZE = 256
# 구조체: Magic(4) + Info(1) + Len(2) + Msg(249) = 256 bytes
STRUCT_FMT = '<IBH249s' 

def main():
    print("📡 라즈베리파이 송신 전용 모드 (Pi -> STM32)")
    
    # USB 장치 찾기
    dev = usb.core.find(idVendor=VID, idProduct=PID)
    if dev is None:
        print("❌ 장치(Vendor)를 찾을 수 없습니다. 연결을 확인하세요.")
        return

    # 설정 적용
    # 리눅스에서 커널 드라이버가 잡고 있으면 해제 필요할 수 있음
    cfg = dev.get_active_configuration()
    intf = next((i for i in cfg if i.bInterfaceClass == 0xFF), None)
    
    # if dev.is_kernel_driver_active(intf.bInterfaceNumber):
    #     try:
    #         dev.detach_kernel_driver(intf.bInterfaceNumber)
    #     except usb.core.USBError as e:
    #         print(f"드라이버 분리 경고: {e}")

    # ------------------------------------------------
    # 2. Endpoint 찾기 (OUT만 필요함)
    # ------------------------------------------------
    # 데이터를 내보낼 구멍(OUT Endpoint)만 찾으면 됩니다.
    ep_out = usb.util.find_descriptor(
        intf, 
        custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT
    )

    if ep_out is None:
        print("❌ OUT Endpoint를 찾을 수 없습니다.")
        return

    print("🚀 데이터 전송 시작... (Ctrl+C로 중단)")

    count = 0
    
    while True:
        try:
            # ------------------------------------------------
            # 3. 데이터 생성 및 패킹
            # ------------------------------------------------
            # 보낼 메시지 (예: "CMD_TEST_01")
            msg_str = f"CMD_TEST_{count}"
            msg_bytes = msg_str.encode('utf-8')
            
            # 구조체 필드 채우기
            # magic=0x12345678, info=0x01, length=문자열길이, data=문자열
            # (구조체 내용물은 STM32가 해석하는 방식에 맞춰 수정하세요)
            magic_num = 0x12345678
            info_val  = 0x01
            payload = struct.pack(STRUCT_FMT, magic_num, info_val, len(msg_bytes), msg_bytes)

            # ------------------------------------------------
            # 4. 전송 (Write)
            # ------------------------------------------------
            # read() 없이 write()만 수행합니다.
            dev.write(ep_out, payload)
            
            print(f"📤 [전송 #{count}] \"{msg_str}\" ({len(payload)} bytes sent)")
            
            count += 1
            time.sleep(1.0) # 1초 간격 전송

        except usb.core.USBError as e:
            print(f"⚠️ USB 에러 발생: {e}")
            # 에러 발생 시 잠시 대기 후 재시도
            time.sleep(1)
            
        except KeyboardInterrupt:
            print("\n🛑 송신을 중단합니다.")
            break

if __name__ == "__main__":
    main()