import usb.core
import usb.util
import struct
import sys
import time

# ==========================================
# 사용자 설정
# ==========================================
VID = 0xCAFE 
PID = 0x4000
PACKET_SIZE = 256       # 펌웨어 고정 사이즈
STRUCT_FMT = '<IBH249s' # 구조체 포맷

def main():
    print("📡 [STM32 모니터링] 연결 시도 중...")
    
    # 1. 장치 찾기
    dev = usb.core.find(idVendor=VID, idProduct=PID)
    if dev is None:
        print("❌ 장치 없음 (VID/PID 확인 필요)")
        return

    # 2. [중요] Vendor 인터페이스(0xFF) 찾기 (사용자님 코드 방식 적용)
    cfg = dev.get_active_configuration()
    
    # 클래스가 0xFF(Vendor Specific)인 인터페이스만 골라냄
    intf = next((i for i in cfg if i.bInterfaceClass == 0xFF), None)
    
    if intf is None:
        print("❌ Vendor Interface(0xFF)를 찾을 수 없습니다.")
        return

    print(f"✅ 장치 연결 성공! (Interface: {intf.bInterfaceNumber})")

    # # 커널 드라이버 분리 (Windows에서는 보통 무시됨)
    # if dev.is_kernel_driver_active(intf.bInterfaceNumber):
    #     try:
    #         dev.detach_kernel_driver(intf.bInterfaceNumber)
    #     except usb.core.USBError:
    #         pass

    # 3. 엔드포인트 찾기
    ep_in = usb.util.find_descriptor(
        intf,
        custom_match=lambda e: usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN
    )
    
    if ep_in is None:
        print("❌ IN Endpoint를 찾을 수 없습니다.")
        return

    print(f"👂 데이터 수신 대기 중... (Ctrl+C로 종료)")
    print("=" * 60)

    while True:
        try:
            # 4. 데이터 읽기 (타임아웃은 넉넉히)
            data = dev.read(ep_in, PACKET_SIZE, timeout=2000)
            
            # --- 여기서부터 출력 포맷팅 ---
            timestamp = time.strftime("%H:%M:%S")
            
            # (1) Raw 데이터 정보
            print(f"[{timestamp}] 📦 수신: {len(data)} bytes")
            
            # (2) Hex 덤프 (앞 20바이트만)
            # 받은 데이터가 어떤 값인지 눈으로 확인하기 위함
            hex_str = " ".join([f"{b:02X}" for b in data[:20]])
            if len(data) > 20: hex_str += " ..."
            print(f"   ↳ Raw Hex : {hex_str}")

            # (3) 구조체 디코딩 시도 (패킷 사이즈가 256일 때)
            if len(data) == PACKET_SIZE:
                try:
                    magic, info, length, payload = struct.unpack(STRUCT_FMT, data)
                    
                    # 유효한 데이터만 잘라내기
                    valid_msg = payload[:length]
                    
                    print(f"   ↳ [구조체] Magic:0x{magic:08X} | Info:0x{info:02X} | Len:{length}")
                    
                    # 내용이 문자열 같으면 문자열로 출력
                    try:
                        decoded_str = valid_msg.decode('utf-8', errors='strict')
                        # 제어 문자 제외하고 출력 가능한지 확인
                        if decoded_str.isprintable() or '\n' in decoded_str: 
                             print(f"   ↳ [메시지] \"{decoded_str}\"")
                    except:
                        # 문자열이 아니면 그냥 Hex로 표시
                        print(f"   ↳ [데이터] {list(valid_msg)}")
                        
                except Exception as e:
                    print(f"   ↳ 구조체 해석 에러: {e}")
            
            # (4) 그냥 문자열일 경우 (구조체 아님)
            else:
                 try:
                    msg = data.tobytes().decode('utf-8')
                    print(f"   ↳ [단순문자열] \"{msg}\"")
                 except:
                    pass

            print("-" * 60)

        except usb.core.USBError as e:
            # 타임아웃(110)은 데이터가 없는 정상이므로 무시하고 계속 루프
            if e.errno == 110: 
                continue
            
            print(f"⚠️ 에러 발생: {e}")
            # 치명적이지 않은 에러면 잠시 대기 후 재시도
            time.sleep(0.5)
            
        except KeyboardInterrupt:
            print("\n🛑 프로그램 종료")
            break

if __name__ == "__main__":
    main()