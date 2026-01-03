import struct
import sys
import time
import os

# [설정] common.h와 일치해야 합니다.
MAGIC_NUMBER = 0xDEADBEEF 
PIPE_PATH = "/tmp/fake_usb"
PACKET_FMT = '<IBH249s' 

TYPE_S_NODE = 1
TYPE_C_NODE = 0

def make_info(ver=1, delay=False, start=False, is_end=False, node_type=TYPE_S_NODE):
    info = (ver << 4)
    if delay: info |= (1 << 3)
    if start: info |= (1 << 2)
    if is_end: info |= (1 << 1)
    if node_type: info |= 1 
    return info

def create_packet(cmd, is_c_node=False, is_delay=False):
    cmd_bytes = cmd.encode('utf-8')
    cmd_len = len(cmd_bytes)
    node_type = TYPE_C_NODE if is_c_node else TYPE_S_NODE
    info = make_info(delay=is_delay, node_type=node_type)
    return struct.pack(PACKET_FMT, MAGIC_NUMBER, info, cmd_len, cmd_bytes)

def run_test():
    if not os.path.exists(PIPE_PATH):
        print(f"Error: {PIPE_PATH} 파이프가 없습니다.")
        return

    print(f">> 시나리오 시작: 가상의 로봇 부팅 시퀀스")
    
    try:
        with open(PIPE_PATH, "wb", buffering=0) as pipe:
            
            def send(cmd, c_node=False, delay=False, desc=""):
                # 패킷 생성
                pkt = create_packet(cmd, is_c_node=c_node, is_delay=delay)
                
                # 로그 출력
                type_str = "C-Node(Back)" if c_node else ("DELAY" if delay else "S-Node(Sync)")
                print(f"[SEND] {type_str:12} : {cmd if not delay else cmd+' sec'} | {desc}")
                
                pipe.write(pkt)
                pipe.flush()
                # 패킷 간 최소한의 간격 (데이터 뭉침 방지용)
                time.sleep(0.1)

            # ========================================================
            # [시나리오] 로봇 시스템 부팅 프로세스
            # ========================================================

            # 1. [S-Node] 초기화 및 경로 설정 (동기화 테스트)
            send("mkdir -p /tmp/robot_sys", desc="작업 폴더 생성")
            send("cd /tmp/robot_sys", desc="작업 폴더 이동 (경로 동기화)")
            send("pwd", c_node=True, desc="현재 경로 확인") 
            
            # 2. [C-Node] 로그 모니터링 데몬 실행 (백그라운드)
            # --> 얘는 혼자 돌게 두고 바로 다음으로 넘어가야 함
            send("sleep 4; echo '>> [LOG_DAEMON] 로그 시스템 준비 완료'", 
                 c_node=True, desc="백그라운드 로그 데몬 시작")

            # 3. [S-Node] 환경 변수 설정 (User 쉘 상태 변경)
            send("export ROBOT_MODE=simulation", desc="환경변수 설정")
            send("echo \"Current Mode: $ROBOT_MODE\"", desc="환경변수 확인")

            # 4. [D-Node] 하드웨어 예열 시간 (시뮬레이터 자체 지연)
            # --> 여기서 시뮬레이터가 2초간 멈춰야 함 (로그 데몬은 계속 돌고 있음)
            send("2", delay=True, desc="하드웨어 예열 대기 (Block)")

            # 5. [S-Node + sudo] 루트 권한으로 드라이버 로드
            # --> 루트 쉘로 스위칭 + 경로(/tmp/robot_sys) 유지 확인
            send("sudo touch driver_loaded.lock", desc="루트 권한으로 락파일 생성")
            send("ls -l driver_loaded.lock", desc="파일 생성 확인")

            # 6. [S-Node] 메인 로직 실행 (Blocking)
            # --> 이 명령이 끝날 때까지 다음 명령 실행 안 됨
            send("echo '>> [MAIN] 시스템 점검 중...'; sleep 2; echo '>> [MAIN] 점검 완료'", 
                 desc="메인 시스템 점검 (2초 소요)")

            # 7. [C-Node] 최종 앱 실행 (병렬)
            send("echo '>> [LAUNCH] 로봇 제어 프로그램 실행!'", c_node=True, desc="최종 실행")

            # 8. 종료 확인
            send("echo '==== 시나리오 종료 (User Shell) ===='", desc="피드백")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    run_test()