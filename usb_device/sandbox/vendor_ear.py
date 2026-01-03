import usb.core
import usb.util
import sys
import time
import struct
import usb.backend.libusb1

# ==========================================
# 사용자 설정
# ==========================================
VENDOR_ID = 0xCAFE
PRODUCT_ID = 0x4000
READ_SIZE = 1024       # 한 번에 읽을 최대 바이트 수 (넉넉하게 설정)
TIMEOUT_MS = 100       # 타임아웃 (짧게 설정해서 루프를 빠르게 돌림)

# 이전에 알려주신 구조체 포맷 (참고용 디코딩)
# 4byte(Magic) + 1byte(Info) + 2byte(Len) + 249byte(Payload)
STRUCT_FMT = '<IBH249s' 
EXPECTED_SIZE = 256

def get_backend():
    # Windows에서 libusb-1.0.dll 강제 로드
    try:
        return usb.backend.libusb1.get_backend(find_library=lambda x: "libusb-1.0.dll")
    except:
        return None

def main():
    print("📡 [USB Monitor] STM32 데이터 수신 대기 중...")
    
    # 1. 장치 찾기
    backend = get_backend()
    dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID, backend=backend)

    if dev is None:
        # 백엔드 지정 없이 재시도
        dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
    
    if dev is None:
        print("❌ 장치를 찾을 수 없습니다.")
        print("   1. 장치가 연결되었는지 확인하세요.")
        print("   2. Zadig로 드라이버가 WinUSB로 설정되었는지 확인하세요.")
        print("   3. libusb-1.0.dll 파일이 같은 폴더에 있는지 확인하세요.")
        return

    # 2. 초기화
    try:
        if dev.is_kernel_driver_active(0):
            dev.detach_kernel_driver(0)
    except NotImplementedError:
        pass # Windows에서는 무시

    dev.set_configuration()
    cfg = dev.get_active_configuration()
    intf = cfg[(0,0)]

    # IN Endpoint (수신용) 찾기
    ep_in = usb.util.find_descriptor(
        intf,
        custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN
    )

    if not ep_in:
        print("❌ IN Endpoint를 찾을 수 없습니다.")
        return

    print(f"✅ 장치 연결됨 (VID:0x{VENDOR_ID:04X} PID:0x{PRODUCT_ID:04X})")
    print(f"👂 수신 대기 시작 (Ctrl+C로 종료)...")
    print("=" * 60)

    # 3. 무한 루프 (Polling)
    while True:
        try:
            # 데이터 읽기 시도
            data = dev.read(ep_in, READ_SIZE, timeout=TIMEOUT_MS)
            
            # --- 데이터가 들어왔을 때만 실행되는 블록 ---
            timestamp = time.strftime("%H:%M:%S")
            print(f"[{timestamp}] 📦 수신: {len(data)} bytes")
            
            # (1) Hex 덤프 출력 (구조체 분석용)
            hex_str = " ".join([f"{b:02X}" for b in data[:16]]) # 앞 16바이트만 표시
            if len(data) > 16: hex_str += " ..."
            print(f"   ↳ Hex : {hex_str}")

            # (2) 문자열 디코딩 시도 (텍스트 메시지용)
            try:
                # 널 문자(\x00) 뒤쪽 쓰레기값 제거 후 디코딩
                clean_bytes = bytes(data).split(b'\x00')[0]
                text_msg = clean_bytes.decode('utf-8')
                if text_msg.isprintable() and len(text_msg) > 0:
                    print(f"   ↳ Text: \"{text_msg}\"")
            except:
                pass # 디코딩 실패하면 출력 안 함

            # (3) 알려주신 구조체 포맷(256byte)과 크기가 같으면 구조체로 해석
            if len(data) == EXPECTED_SIZE:
                try:
                    magic, info, length, payload = struct.unpack(STRUCT_FMT, data)
                    print(f"   ↳ Struct 감지! Magic=0x{magic:08X}, Info=0x{info:02X}, DataLen={length}")
                except:
                    pass

            print("-" * 60)

        except usb.core.USBError as e:
            # 타임아웃(Timeout)은 에러가 아니라 "아직 데이터 없음" 상태임
            if e.errno == 110: 
                continue # 다시 루프 처음으로
            elif e.errno == 10060: # Windows Socket Timeout 코드
                continue
            else:
                print(f"⚠️ USB 에러: {e}")
                time.sleep(1) # 에러 발생 시 잠시 대기
        
        except KeyboardInterrupt:
            print("\n🛑 프로그램 종료")
            break

if __name__ == "__main__":
    main()