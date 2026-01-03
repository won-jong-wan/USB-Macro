import usb.core
import usb.util
import struct
import sys
import ctypes

# ==========================================
# 1. 설정 (사용자 환경에 맞게 수정)
# ==========================================
VENDOR_ID = 0xCAFE
PRODUCT_ID = 0x4000
PACKET_SIZE = 256  # 펌웨어가 기대하는 고정 크기
TIMEOUT_MS = 2000

# 공유해주신 구조체 포맷: <IBH249s 
# I: uint32 (4byte) - Magic/Header
# B: uint8  (1byte) - Info/ID
# H: uint16 (2byte) - Length
# 249s: char[249]   - Payload
STRUCT_FMT = '<IBH249s' 

def find_device():
    # Windows에서 libusb 백엔드 강제 로드 시도 (DLL이 같은 폴더에 있어야 함)
    import usb.backend.libusb1
    backend = usb.backend.libusb1.get_backend(find_library=lambda x: "libusb-1.0.dll")
    
    dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID, backend=backend)
    if dev is None:
        # 백엔드 지정 없이 재시도 (혹시 이미 path에 있을 경우)
        dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
        
    if dev is None:
        raise ValueError("장치를 찾을 수 없습니다. (DLL 파일 위치와 Zadig 드라이버 확인 필요)")
    
    dev.set_configuration()
    return dev

def get_endpoints(dev):
    cfg = dev.get_active_configuration()
    intf = cfg[(0,0)]

    ep_out = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
    ep_in = usb.util.find_descriptor(intf, custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
    return ep_out, ep_in

def send_and_receive(name, magic, info, payload_bytes, ep_out, ep_in):
    print(f"\n--- [Test] {name} ---")
    
    # 1. 데이터 패킹 (항상 256바이트로 맞춤)
    payload_len = len(payload_bytes)
    
    # 249바이트보다 길면 자름
    if payload_len > 249:
        payload_bytes = payload_bytes[:249]
        payload_len = 249

    # 구조체 생성 (빈 공간은 자동으로 0으로 채워짐)
    # 249s 포맷은 남은 공간을 null byte로 채워줍니다.
    packet = struct.pack(STRUCT_FMT, magic, info, payload_len, payload_bytes)
    
    print(f" -> 전송: Magic=0x{magic:08X}, Info=0x{info:02X}, Len={payload_len}, Bytes={list(payload_bytes[:10])}...")
    
    try:
        # OUT 전송
        ep_out.write(packet, timeout=TIMEOUT_MS)

        # IN 수신 (Echo 대기)
        # STM32가 받은 걸 그대로 돌려준다고 가정
        data = ep_in.read(PACKET_SIZE, timeout=TIMEOUT_MS)
        
        if len(data) == PACKET_SIZE:
            # 언패킹
            r_magic, r_info, r_len, r_payload = struct.unpack(STRUCT_FMT, data)
            
            # 실제 유효한 데이터만 잘라내기
            valid_payload = r_payload[:r_len]
            
            print(f" <- 수신: Magic=0x{r_magic:08X}, Valid Len={r_len}")
            
            # 보낸 것과 받은 내용 비교
            if valid_payload == payload_bytes[:payload_len]:
                 print(" ✅ 성공: 데이터 일치")
            else:
                 print(" ❌ 실패: 데이터 불일치")
                 try:
                     print(f"    Recv Msg: {valid_payload.decode('utf-8')}")
                 except:
                     print(f"    Recv Hex: {list(valid_payload)}")
        else:
            print(f" ⚠️ 경고: 패킷 크기 이상함 ({len(data)} bytes)")

    except usb.core.USBError as e:
        print(f" 🚨 에러: {e}")

# ==========================================
# 메인 실행
# ==========================================
if __name__ == "__main__":
    try:
        dev = find_device()
        ep_out, ep_in = get_endpoints(dev)
        print("USB 연결 성공. 테스트 시작...\n")

        # CASE 1: 짧은 문자열 보내기
        # Magic: 0x11111111, Info: 0x01
        msg1 = b"Hello STM32!"
        send_and_receive("짧은 문자열", 0x11111111, 0x01, msg1, ep_out, ep_in)

        # CASE 2: 구조체 데이터인 척 보내기
        # 실제로 구조체를 보내지만, USB상으로는 결국 바이트 배열입니다.
        # Magic: 0x22222222, Info: 0x02 (구조체 ID라고 가정)
        # 예: x, y 좌표값 (uint16 * 2)을 보낸다고 가정
        struct_data = struct.pack('<HH', 1024, 2048) # 1024, 2048이라는 좌표값
        send_and_receive("좌표 구조체(4 bytes)", 0x22222222, 0x02, struct_data, ep_out, ep_in)

        # CASE 3: 긴 문자열 보내기
        msg3 = b"A" * 200 # 200바이트 꽉 채워서 보내기
        send_and_receive("긴 데이터(200 bytes)", 0x33333333, 0x03, msg3, ep_out, ep_in)

    except Exception as e:
        print(f"\n[Fatal Error] {e}")
        print("팁: 'Operation not supported' 에러라면 libusb-1.0.dll 파일이 스크립트 폴더에 있는지 확인하세요.")