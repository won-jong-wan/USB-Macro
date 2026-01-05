#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h> // 사용자 정보(홈디렉토리 등) 조회용
#include "executor.h"
#include <time.h> 
#include <poll.h>

#define MAX_BG_JOBS 10
#define ENV_FILE_USER "/tmp/stm32_env_user.sh"
#define ENV_FILE_ROOT "/tmp/stm32_env_root.sh"

// 백그라운드 프로세스 PID 저장소
static pid_t bg_pids[MAX_BG_JOBS];
static int bg_count = 0;

// [변경] 전역 변수로 관리 (자동 감지된 값 저장)
static uid_t target_uid = 0;
static gid_t target_gid = 0;
static char target_home[256] = "/root"; // 기본값

// 쉘 관리 구조체
typedef struct {
    pid_t pid;
    int fd_in;  
    int fd_out; 
    char current_dir[1024]; 
} ShellProc;

static ShellProc user_sh;
static ShellProc root_sh;

// [내부 함수] 쉘 프로세스 생성
static int spawn_shell(ShellProc *sh, int is_root) {
    int pipe_in[2], pipe_out[2];
    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0) return -1;

    pid_t pid = fork();
    if (pid < 0) return -1;

    if (pid == 0) {
        // === 자식 프로세스 (Bash) ===
        
        // 1. 파이프 연결
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        dup2(pipe_out[1], STDERR_FILENO);
        
        close(pipe_in[0]); close(pipe_in[1]);
        close(pipe_out[0]); close(pipe_out[1]);

        // 2. 권한 및 환경 설정
        if (!is_root) {
            // [핵심] 일반 유저인 경우, 감지된 UID/GID로 변신
            // 홈 디렉토리 환경변수도 강제로 맞춰줌 (안하면 /root로 잡힐 수 있음)
            setenv("HOME", target_home, 1);
            setenv("USER", getenv("SUDO_USER") ? getenv("SUDO_USER") : "user", 1);
            
            // 그룹 먼저 바꾸고 유저 바꿔야 함
            setresgid(target_gid, target_gid, target_gid);
            setresuid(target_uid, target_uid, target_uid);
            
            // 작업 디렉토리도 홈으로 이동
            chdir(target_home);
        } else {
            // 루트인 경우
            setenv("HOME", "/root", 1);
            chdir("/root");
        }

        // 3. 쉘 실행 (설정 파일 로딩 안 함)
        execl("/bin/bash", "bash", "--norc", NULL);
        exit(1);
    }

    // === 부모 프로세스 ===
    close(pipe_in[0]);
    close(pipe_out[1]);
    
    sh->pid = pid;
    sh->fd_in = pipe_in[1];
    sh->fd_out = pipe_out[0];

    // 초기 경로 설정 (쉘이 시작된 위치)
    if (is_root) strcpy(sh->current_dir, "/root");
    else strcpy(sh->current_dir, target_home);

    dprintf(sh->fd_in, "export PS1=''\n");
    return 0;
}

// [공개 함수] 초기화 (여기서 유저 감지!)
int init_executor() {
    if (geteuid() != 0) {
        fprintf(stderr, ">> [ERROR] sudo 권한이 필요합니다.\n");
        return -1;
    }

    char *sudo_user = getenv("SUDO_USER"); // 직접 sudo 실행 시
    char *daemon_user = getenv("DAEMON_USER"); // 서비스 실행 시

    struct passwd *pw = NULL;

    if (sudo_user) {
        // 1. 직접 sudo로 실행했을 경우 (SUDO_USER 사용)
        pw = getpwnam(sudo_user);
    } 
    else if (daemon_user) {
        // 2. 서비스로 실행했을 경우 (DAEMON_USER 사용)
        pw = getpwnam(daemon_user);
    }

    if (pw) {
        // 운영체제에서 정보를 성공적으로 가져옴
        target_uid = pw->pw_uid;
        target_gid = pw->pw_gid;
        strncpy(target_home, pw->pw_dir, sizeof(target_home) - 1);

        printf(">> [INIT] 사용자 모드 감지: %s (UID:%d, HOME:%s)\n", 
            pw->pw_name, target_uid, target_home);
    }else {
        printf(">> [WARN] SUDO 실행 아님. 기본값(UID:1000)을 사용합니다.\n");
        target_uid = 1000;
        target_gid = 1000;
        strcpy(target_home, "/home/pi"); // 기본값 하드코딩 (필요시 수정)
    }

    printf(">> [INIT] 쉘 프로세스 생성 중...\n");
    if (spawn_shell(&root_sh, 1) < 0) return -1;
    if (spawn_shell(&user_sh, 0) < 0) return -1;

    return 0;
}

// S-Node 내부 로직
static void internal_exec_s_node(const char *cmd) {
    int is_sudo = 0;
    const char *actual_cmd = cmd;

    if (strncmp(cmd, "sudo ", 5) == 0) {
        is_sudo = 1;
        actual_cmd = cmd + 5;
    }

    ShellProc *active = is_sudo ? &root_sh : &user_sh;
    ShellProc *passive = is_sudo ? &user_sh : &root_sh;

    const char *env_file = is_sudo ? ENV_FILE_ROOT : ENV_FILE_USER;

    printf(">> [S-NODE] (%s) %s\n", is_sudo ? "ROOT" : "USER", actual_cmd);

    // 1. 명령 전송 + 종료 마커 요청
    dprintf(active->fd_in, "%s; export -p > %s; echo \"__END__:$?:$PWD\"\n", 
            actual_cmd, env_file);

    // 2. 결과 읽기 (Poll을 이용한 타임아웃 감시) 
    // 혹시 C 가야할 걸 S로 보내는 실수 방지
    char buffer[4096];
    char new_dir[1024] = {0};
    
    // 시간 측정 변수
    time_t start_time = time(NULL);
    time_t last_warn_time = start_time;
    int warn_interval_sec = 60; // 1분마다 경고

    struct pollfd pfd;
    pfd.fd = active->fd_out;
    pfd.events = POLLIN;

    while (1) {
        // 100ms(0.1초) 기다림
        int ret = poll(&pfd, 1, 100);

        if (ret < 0) {
            perror(">> [ERROR] Poll Failed");
            break;
        }

        if (ret == 0) {
            // === Timeout (데이터 없음, 0.1초 경과) ===
            // 여기서 시간 체크를 수행합니다.
            time_t now = time(NULL);
            if (difftime(now, last_warn_time) >= warn_interval_sec) {
                long elapsed = (long)difftime(now, start_time);
                printf("\n>> [WARN] 명령어가 %ld초째 실행 중입니다... (Wait)\n", elapsed);
                last_warn_time = now;
            }
            continue; // 다시 대기하러 감
        }

        if (pfd.revents & POLLIN) {
            // === 데이터 수신 ===
            // fgets 대신 read 사용 (Non-blocking 느낌으로 처리)
            ssize_t len = read(active->fd_out, buffer, sizeof(buffer) - 1);
            if (len <= 0) break; // 쉘이 죽었거나 파이프 끊김

            buffer[len] = '\0'; // 문자열 종료 처리

            // 종료 마커 검사
            char *marker = strstr(buffer, "__END__:");
            if (marker) {
                // 파싱 로직 (기존과 동일)
                char *path_start = strchr(marker + 8, ':');
                if (path_start) {
                    path_start++; // 경로 시작점
                    // 뒤에 따라오는 개행이나 쓰레기값 제거
                    char *newline = strchr(path_start, '\n');
                    if (newline) *newline = '\0';
                    
                    strcpy(new_dir, path_start);
                }
                
                // 마커 부분은 화면에 출력 안 하고 루프 종료
                // (마커 앞부분에 출력이 섞여있을 수 있으므로 마커 전까지만 출력하는 디테일이 필요하지만
                //  간단히 마커 위치를 NULL로 바꿔서 앞부분만 출력하게 함)
                *marker = '\0'; 
                printf("%s", buffer);
                break; 
            }

            // 일반 출력은 그냥 화면에 표시
            printf("%s", buffer);
        }
    }

    // 3. 경로 동기화 (기존과 동일)
    if (strlen(new_dir) > 0 && strcmp(new_dir, passive->current_dir) != 0) {
        printf(">> [SYNC] 경로 동기화 %s -> %s\n", is_sudo ? "User" : "Root", new_dir);
        dprintf(passive->fd_in, "cd %s\n", new_dir);
        strcpy(active->current_dir, new_dir);
        strcpy(passive->current_dir, new_dir);
    }
}

// C-Node 내부 로직
static void internal_exec_c_node(const char *cmd) {
    // 슬롯 꽉 찼는지 확인 (간단한 원형 큐 관리 혹은 무시)
    if (bg_count >= MAX_BG_JOBS) {
        printf(">> [WARN] 백그라운드 슬롯이 가득 찼습니다. (무시됨: %s)\n", cmd);
        return;
    }

    int is_sudo = 0;
    const char *actual_cmd = cmd;

    if (strncmp(cmd, "sudo ", 5) == 0) {
        is_sudo = 1;
        actual_cmd = cmd + 5;
    }

    const char *env_file = is_sudo ? ENV_FILE_ROOT : ENV_FILE_USER;

    printf(">> [C-NODE] Background (%s): %s\n", is_sudo ? "ROOT" : "USER", actual_cmd);

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork Failed");
        return;
    }

    if (pid == 0) {
        // === 자식 프로세스 ===
        
        // [핵심] 새로운 세션 생성 -> 프로세스 그룹 리더가 됨
        // 나중에 kill(-pid)로 이 프로세스와 그 자식들(ROS 노드들)을 한방에 죽일 수 있음
        setsid(); 

        if (!is_sudo) {
            // 사용자 권한 및 환경 설정
            setenv("HOME", target_home, 1);
            
            // 그룹 먼저 변경 후 유저 변경 (순서 중요)
            setresgid(target_gid, target_gid, target_gid);
            setresuid(target_uid, target_uid, target_uid);
            
            // 현재 S-Node 사용자가 보고 있는 경로와 맞춰주면 좋음 (옵션)
            chdir(user_sh.current_dir);
        } else {
            setenv("HOME", "/root", 1);
            chdir(root_sh.current_dir);
        }

        char final_cmd[2048];
        snprintf(final_cmd, sizeof(final_cmd), 
                 "[ -f %s ] && . %s; %s", 
                 env_file, env_file, actual_cmd);

        // 쉘을 통해 실행 (환경변수 확장 등을 위해)
        execl("/bin/bash", "bash", "-c", final_cmd, NULL);
        
        // exec 실패 시
        exit(1); 
    } 
    else {
        // === 부모 프로세스 ===
        // 생성된 PID를 배열에 저장
        bg_pids[bg_count++] = pid;
        printf("   -> Process Launched (PID: %d)\n", pid);
    }
}

int execute_command(const char *cmd, int is_background) {
    if (is_background) {
        internal_exec_c_node(cmd);
    } else {
        internal_exec_s_node(cmd);
    }
    return 0;
}

void terminate_background_jobs(void) {
    if (bg_count == 0) return;

    printf(">> [CLEANUP] 실행 중인 백그라운드 작업 %d개를 종료합니다...\n", bg_count);
    
    for (int i = 0; i < bg_count; i++) {
        if (bg_pids[i] > 0) {
            // [핵심] 음수(-) PID를 사용하여 프로세스 '그룹' 전체 종료
            // ROS2 launch 처럼 자식 프로세스를 많이 만드는 경우 필수입니다.
            kill(-bg_pids[i], SIGTERM); 
            printf("   -> Kill PID Group %d\n", bg_pids[i]);
            
            // 좀비 프로세스 방지를 위해 회수 (Non-blocking)
            waitpid(bg_pids[i], NULL, WNOHANG);
        }
    }
    
    // 목록 초기화
    bg_count = 0;
    memset(bg_pids, 0, sizeof(bg_pids));
}