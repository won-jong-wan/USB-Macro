import usb.core
import usb.util
import struct
import sys
import time

# ------------------------------------------------
# 1. 설정 (Vendor ID, Product ID, 패킷 크기)
# ------------------------------------------------
VID = 0xCAFE 
PID = 0x4000
PACKET_SIZE = 256

# 구조체 포맷: Little Endian(<)
# I (4 bytes) : Magic Number 또는 ID
# B (1 byte)  : Info/Status
# H (2 bytes) : Data Length
# 249s (249 bytes) : Payload (문자열 또는 데이터)
# 총합 = 4 + 1 + 2 + 249 = 256 bytes
STRUCT_FMT = '<IBH249s' 

def main():
    print("📡 STM32 USB 통신 테스트 (송신 -> 수신 루프)")
    
    # USB 장치 찾기
    dev = usb.core.find(idVendor=VID, idProduct=PID)
    if dev is None:
        print("❌ 장치(Vendor)를 찾을 수 없습니다. 연결을 확인하세요.")
        return

    # 커널 드라이버 분리 (리눅스/맥의 경우 필요할 수 있음)
    cfg = dev.get_active_configuration()
    intf = next((i for i in cfg if i.bInterfaceClass == 0xFF), None)
    
    # if dev.is_kernel_driver_active(intf.bInterfaceNumber):
    #     try:
    #         dev.detach_kernel_driver(intf.bInterfaceNumber)
    #         print("✅ 커널 드라이버 분리 완료")
    #     except usb.core.USBError as e:
    #         print(f"⚠️ 드라이버 분리 경고: {e}")

    # Endpoint 찾기 (IN: 읽기용, OUT: 쓰기용)
    ep_out = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
    ep_in = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)

    if ep_out is None or ep_in is None:
        print("❌ Endpoint를 찾을 수 없습니다.")
        return

    print("🚀 통신 시작 (Ctrl+C로 종료)")

    count = 0
    try:
        while True:
            # ==========================================
            # 1. 송신 (PC -> STM32)
            # ==========================================
            msg_str = f"REQUEST_{count}"
            msg_bytes = msg_str.encode('utf-8')
            
            # 구조체 패킹: Magic(임의), Info(0x10), Len, Data
            tx_payload = struct.pack(STRUCT_FMT, 0x12345678, 0x10, len(msg_bytes), msg_bytes)
            
            dev.write(ep_out, tx_payload)
            print(f"\n📤 [송신 #{count}] \"{msg_str}\" ({len(tx_payload)} bytes)")

            # ==========================================
            # 2. 수신 (STM32 -> PC)
            # ==========================================
            try:
                # 응답 대기 (Timeout 2초)
                rx_data = dev.read(ep_in, PACKET_SIZE, timeout=2000)
                
                if len(rx_data) == PACKET_SIZE:
                    # 구조체 해석 (Unpack)
                    magic, info, length, payload_raw = struct.unpack(STRUCT_FMT, rx_data)
                    
                    # 실제 메시지만 추출 (길이만큼 자르기)
                    # errors='replace'는 깨진 문자가 있을 경우 ?로 표시
                    rx_msg = payload_raw[:length].decode('utf-8', errors='replace')
                    
                    print(f"📥 [수신] Magic: 0x{magic:08X} | Info: 0x{info:02X}")
                    print(f"   └── 메시지: \"{rx_msg}\" (Len: {length})")
                else:
                    print(f"⚠️ 크기 불일치: {len(rx_data)} bytes 수신됨")

            except usb.core.USBError as e:
                if e.errno == 110: # Timeout
                    print("⏳ 응답 없음 (Timeout)")
                else:
                    print(f"❌ 수신 에러: {e}")

            count += 1
            time.sleep(1) # 1초 대기

    except KeyboardInterrupt:
        print("\n🛑 프로그램을 종료합니다.")

if __name__ == "__main__":
    main()