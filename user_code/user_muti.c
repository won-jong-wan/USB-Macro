#include "define.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h> // uint8_t, uint16_t, uint32_t를 사용하기 위함
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
//#define GARA

// MAX_C_NODES : 최대 실행시킬 수 있는 프로세스 개수
// BUFFER_SIZE : 로그를 한 번에 읽을 수 있는 길이(넘어가면 크기 다 읽을 때까지
// 읽음)
#define MAX_C_NODES 10
#define BUFFER_SIZE 1024

#define GET_VERSION(info) (((info) >> 4) & 0x0F) // 하위 4비트만 출력
#define GET_DELAY(info) (((info) >> 3) & 0x01)
#define GET_START(info) (((info) >> 2) & 0x01)   
#define GET_ISEND(info) (((info) >> 1) & 0x01) // 오른쪽으로 2번 시프트, 최하위 1비트 출력
#define GET_TYPE(info) ((info) & 0x01)    // 최하위 1비트만 출력

//#define SET_INFO(version, start, isEnd, type) \
  (((version) << 4) | ((start) << 2) | ((isEnd) << 1) | (type))

#define MAGIC_NUMBER 0xDEADBEEF

typedef struct {
  pid_t pid;
  int pipe_fd; // 파이프 읽기 파일 디스크립터(파이프 번호)
  char command[256];
} BackgroundProcess;

BackgroundProcess bg_processes[MAX_C_NODES];
int bg_count = 0;

static int g_usb_fd = -1;
static pid_t g_user_pid = -1;
static pid_t g_root_pid = -1;
static int g_user_pipe_w = -1;
static int g_root_pipe_w = -1;
static int g_user_log_r = -1;
static int g_root_log_r = -1;

void signal_handler(int signum) {
  printf("\n\n[시그널] Ctrl+C 감지 (시그널: %d)\n", signum);
  printf("[정리] 리소스 정리 시작...\n");

  // 1. 백그라운드 프로세스 정리
  printf("[정리] 백그라운드 프로세스 종료 중...\n");
  for (int i = 0; i < bg_count; i++) {
    if (bg_processes[i].pid > 0) {
      printf("  - PID %d 종료 중...\n", bg_processes[i].pid);
      kill(bg_processes[i].pid, SIGTERM);
    }
    if (bg_processes[i].pipe_fd >= 0) {
      close(bg_processes[i].pipe_fd);
      bg_processes[i].pipe_fd = -1;
    }
  }

  // 2. 자식 프로세스 종료
  if (g_user_pid > 0) {
    printf("[정리] $ 프로세스 (PID %d) 종료 중...\n", g_user_pid);
    kill(g_user_pid, SIGTERM);
    waitpid(g_user_pid, NULL, WNOHANG);
  }

  if (g_root_pid > 0) {
    printf("[정리] # 프로세스 (PID %d) 종료 중...\n", g_root_pid);
    kill(g_root_pid, SIGTERM);
    waitpid(g_root_pid, NULL, WNOHANG);
  }

  // 3. 파이프 파일 디스크립터 닫기
  if (g_user_pipe_w >= 0) {
    close(g_user_pipe_w);
    printf("[정리] user_pipe 닫음\n");
  }
  if (g_root_pipe_w >= 0) {
    close(g_root_pipe_w);
    printf("[정리] root_pipe 닫음\n");
  }
  if (g_user_log_r >= 0) {
    close(g_user_log_r);
    printf("[정리] user_log_pipe 닫음\n");
  }
  if (g_root_log_r >= 0) {
    close(g_root_log_r);
    printf("[정리] root_log_pipe 닫음\n");
  }

  // 4. USB 디바이스 닫기 ⭐ 중요!
  if (g_usb_fd >= 0) {
    usb_close_device(g_usb_fd);
    printf("[정리] USB 디바이스 닫음 (fd: %d)\n", g_usb_fd);
    g_usb_fd = -1;
  }

  printf("[정리] 모든 리소스 정리 완료\n");
  printf("[종료] 프로그램 종료\n\n");

  exit(0);
}

int execute_S_node(const char *command) {
  printf("\n[S노드] 실행 : %s\n", command);
  printf("[S노드] 명령어 완료 대기...\n");

  pid_t pid = fork();
  // pid_t는 프로세스 ID를 담는다는 것을 표현하기 위함
  // unistd.h에 포함

  if (pid == 0) {
    char full_cmd[1024];
    
    if(strncmp(command, "ros2 ", 5) == 0)
    {
      snprintf(full_cmd, sizeof(full_cmd), 
             "source /opt/ros/humble/setup.bash && "
             "source /home/burger/turtlebot3_ws/install/setup.bash && "
             "export ROS_DOMAIN_ID=50 && "
             "export LDS_MODEL=rplidar_c1 && "
             "export TURTLEBOT3_MODEL=burger && "
             "%s", 
             command);
    }
    else
    {
      snprintf(full_cmd, sizeof(full_cmd), "%s", command);
    }
    execlp("bash", "bash", "-c", full_cmd, NULL);
    // execlp의 p는 PATH를 의미 → PATH에서 자동으로 찾음
    // 첫 번째 bash : PATH 환경변수에서 "bash" 프로그램 찾기
    // 두 번째 bash : 그냥 이름임
    // -c : bash 옵션(쉘 명령어 실행)
    // node->command : 실행시킬 명령어

    perror("exec 실패");
    exit(1);
  } else if (pid > 0) {
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    // WIFEXITED(status) : 정상 종료를 했는가
    // WEXITSTATUS(status) : 종료 코드가 어떤 것인지
    {
      printf("[S노드] 완료\n");
      return 0;
    } else {
      printf("[S노드] 실패\n");
      return 1;
    }
  } else {
    perror("fork 실패");
    return -1;
  }
}

int execute_C_node(const char *command) {
  printf("\n[C노드] 실행 : %s\n", command);

  if (bg_count >= MAX_C_NODES) {
    printf("[C노드] 최대 백그라운드 프로세스 개수 초과\n");
    return -1;
  }

  // 파이프 파일 디스크립터 2개를 담을 배열 생성
  // 파이프 생성
  int pipefd[2];
  if (pipe(pipefd) == -1) // 파이프를 생성하고 제대로 생성됐는지 확인
  {
    perror("pipe 생성 실패");
    return -1;
  }

  pid_t pid = fork();

  if (pid == 0) {
    // 파일 디스크립터를 닫음(읽기용)
    // 자식은 명령어 쓰기만 하니까 읽기가 필요 없음
    close(pipefd[0]);

    // dup2 : duplicate file descriptor 2(파일 디스크립터 복제 함수)
    // STDOUT_FILENO : 일반적인 모든 출력
    // STDERR_FILENO : 에러/경고 메시지
    dup2(pipefd[1], STDOUT_FILENO);
    dup2(pipefd[1], STDERR_FILENO);
    // 파이프로 출력 방향을 전환시킴

    close(pipefd[0]);

    char full_cmd[1024];
    if(strncmp(command, "ros2 ", 5) == 0)
    {
      snprintf(full_cmd, sizeof(full_cmd), 
             "source /opt/ros/humble/setup.bash && "
             "source /home/burger/turtlebot3_ws/install/setup.bash && "
             "export ROS_DOMAIN_ID=50 && "
             "export LDS_MODEL=rplidar_c1 && "
             "export TURTLEBOT3_MODEL=burger && "
             "%s", 
             command);
    }
    else
    {
      snprintf(full_cmd, sizeof(full_cmd), "%s", command);
    } 
    execlp("bash", "bash", "-c", full_cmd, NULL);
    perror("exec 실패");
    exit(1);
  } else if (pid > 0) {
    close(pipefd[1]); // 쓰기용 파이프 닫아줌. 부모(메인)는 읽어서 출력만 하니까

    int flags = fcntl(pipefd[0], F_GETFL, 0);
    // fcntl : File CoNToL (파일 제어 함수)
    // 파일 디스크립터의 속성을 읽거나 변경
    // F_GETFL : 모든 옵션을 가져옴
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    // F_SETFL : 플래그를 설정
    // 기존 설정과 함께 논블로킹을 설정해줌
    // 논블로킹 이유 : C노드 명령어가 끝나는 것을 기다리지 않고 다른 명령어를
    // 진행시키기 위함

    // 출력을 위해서 정보를 저장
    bg_processes[bg_count].pid = pid;
    bg_processes[bg_count].pipe_fd = pipefd[0];
    strncpy(bg_processes[bg_count].command, command, 255);
    bg_count++;

    printf("[C노드] ✓ 백그라운드 실행 시작 (대기 안 함)\n");

    return 0; // exit(0) 아님!
  } else {
    perror("fork 실패");
    close(pipefd[0]);
    close(pipefd[1]);
    return -1;
  }
}

// 백그라운드 프로세스의 출력을 읽기 위함
void read_background_output() {
  char buffer[BUFFER_SIZE];

  // 생성된 프로세스만큼 반복
  for (int i = 0; i < bg_count; i++) {
    ssize_t byte_read;

    // 데이터를 읽지 못했을 때 까지, 마지막은 널문자~
    while ((byte_read =
                read(bg_processes[i].pipe_fd, buffer, BUFFER_SIZE - 1)) > 0) {
      buffer[byte_read] = '\0';
      printf("[백그라운드 PID %d] %s", bg_processes[i].pid, buffer);
      fflush(stdout);
      // stdout로 출력을 해주고 fflush로 버퍼를 비워줌
      // 실시간으로 로그가 생길 때마다 출력
    }

    // 종료됐을 때(정상 or 에러)
    if (byte_read == 0 ||
        (byte_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK)) {
      if (byte_read == 0) {
        printf("[백그라운드 PID %d] 프로세스 종료\n", bg_processes[i].pid);
      }
      close(bg_processes[i].pipe_fd);
      bg_processes[i].pipe_fd = -1;
    }
  }
}

// 손상이 있는지, 올바른지 확인하는 부분
int validata_packet(datapacket *pkt) {
  if (pkt->magic != MAGIC_NUMBER) {
    printf("잘못된 매직 넘버 : 0x%X\n", pkt->magic);
    return 0;
  }
  if (pkt->cmd_len > 249) {
    printf("명령어 길이 초과 : %d\n", pkt->cmd_len);
    return 0;
  }
  return 1;
}

// 패킷의 담긴 정보를 출력하고, 명령어를 실행
int process_packet(datapacket *pkt) {
  if (!validata_packet(pkt)) {
    return -1;
  }

  uint8_t version = GET_VERSION(pkt->info);
  uint8_t delay = GET_DELAY(pkt->info);
  uint8_t start = GET_START(pkt->info);
  uint8_t isEnd = GET_ISEND(pkt->info);
  uint8_t type = GET_TYPE(pkt->info);

  printf("패킷 정보\n");
  printf("  Magic   : 0x%X\n", pkt->magic);
  printf("  Version : %d\n", version);
  printf("  Delay   : %s\n", delay ? "지연" : "-");
  printf("  Start   : %s\n", start ? "시작" : "-");
  printf("  IsEnd   : %s\n", isEnd ? "마지막 패킷" : "계속");
  printf("  Type    : %s\n", type ? "S노드" : "C노드");
  printf("  Cmd Len : %d\n", pkt->cmd_len);
  printf("  Command : %s\n", pkt->command);

  int result;

  if(delay == 1)
  {
    int delay_seconds = atoi(pkt->command);
    printf("[지연] %d초 대기 중...\n", delay_seconds);
    sleep(delay_seconds);
    printf("[지연] 완료");
    return 0;
  }
  if (type == 1) {
    result = execute_S_node(pkt->command);
  } else {
    result = execute_C_node(pkt->command);
  }
  return result;
}

void user_process_loop(int pipe_fd)
{
  printf("\n════════════════════════════════════\n");
  printf("[$] $ 프로세스 시작\n");
  printf("    UID: %d (일반 사용자)\n", getuid());
  printf("════════════════════════════════════\n\n");

  setenv("HOME", "/home/burger", 1);
  
  int flags = fcntl(pipe_fd, F_GETFL, 0);
  fcntl(pipe_fd, F_SETFL, flags | O_NONBLOCK);

  while(1)
  {
    datapacket pkt;

    ssize_t n = read(pipe_fd, &pkt, sizeof(datapacket));

    if(n == sizeof(datapacket))
    {
      printf("\n[$] 명령어 수신: %s\n", pkt.command);

      int result = process_packet(&pkt);

      if(result != 0 && GET_TYPE(pkt.info) == 0)
      {
        printf("[$] S노드 실패\n");
      }
    }
    else if(n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
      printf("\n[$] $ 프로세스 종료\n");
      break;
    }

    read_background_output();
    usleep(10000);
  }
}

void root_process_loop(int pipe_fd)
{
  printf("\n════════════════════════════════════\n");
  printf("[#] # 프로세스 시작\n");
  printf("    UID: %d (root)\n", getuid());
  printf("════════════════════════════════════\n\n");

  int flags = fcntl(pipe_fd, F_GETFL, 0);
  fcntl(pipe_fd, F_SETFL, flags | O_NONBLOCK);

  while(1)
  {
    datapacket pkt;

    ssize_t n = read(pipe_fd, &pkt, sizeof(datapacket));

    if(n == sizeof(datapacket))
    {
      printf("\n[#] 명령어 수신: %s\n", pkt.command);
      
      char *cmd = pkt.command;
      if(strncmp(cmd, "sudo ", 5) == 0)
      {
        cmd += 5;
        printf("[#] sudo 제거 후 실행: %s\n", cmd);
        memmove(pkt.command, cmd, strlen(cmd)+1);
        pkt.cmd_len = strlen(pkt.command);
      }

      int result = process_packet(&pkt);

      if(result != 0 && GET_TYPE(pkt.info) == 0)
      {
        printf("[#] S노드 실패\n");
      }
    }
    else if(n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
      printf("\n[#] # 프로세스 종료\n");
      break;
    }
    
    read_background_output();
    usleep(10000);
  }
}

void distribute_commands(int user_fd, int root_fd, int user_log_fd, int root_log_fd)
{
  datapacket packets[256];
  datapacket rx_packets[256];
  int i = 0;
  int packet_count = 0;
  const char *dev_path = "/dev/team_own_stm32";
  int fd;
  fd = usb_open_device(dev_path);
  if (fd < 0)
  {
    close(user_fd);
    close(root_fd);
    close(user_log_fd);
    close(root_log_fd);
    return;
  }

  g_usb_fd = fd;

  i = 0;
  packet_count = 0;

  while (1) {
    if (usb_read_packet(fd, &rx_packets[i]) < 0) {
      perror("read failed");
      break;
    }
    printf("Struct : MAGIC=%d, INFO=0x%x, CMD_LEN=%d, command=%s\n",
           rx_packets[i].magic, rx_packets[i].info, rx_packets[i].cmd_len,
           rx_packets[i].command);

    packet_count++;

    int isend = GET_ISEND(rx_packets[i++].info);

    if (isend == 1)
      break;
  }

  printf("-------  패킷 처리 시작--------------\n");

  for (int i = 0; i < packet_count; i++) {
    printf("패킷 %d / %d 처리 중\n", i + 1, packet_count);

    if(strncmp(rx_packets[i].command, "sudo ", 5) == 0)
    {
      printf("→ [#] 프로세스로 전달\n");
      write(root_fd, &rx_packets[i], sizeof(datapacket));
    }
    else
    {
      printf("→ [$] 프로세스로 전달\n");
      write(user_fd, &rx_packets[i], sizeof(datapacket));
    }

    sleep(1);
  }

  printf("\n======================================\n");
  printf("  모든 패킷 처리 완료\n");

  printf("======================================\n");
  printf("\nC노드 프로세스는 백그라운드에서 계속 실행 중\n");
  printf("확인: ps aux | grep while\n");
  printf("확인: ps aux | grep ping\n");
  printf("메인 프로세스 종료는 Ctrl+c\n");

  char buffer[BUFFER_SIZE];
  while(1)
  {
    ssize_t n;

    while((n = read(user_log_fd, buffer, BUFFER_SIZE-1)) > 0)
    {
        buffer[n] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }

    while((n = read(root_log_fd, buffer, BUFFER_SIZE-1)) > 0)
    {
        buffer[n] = '\0';
        printf("%s", buffer);
        fflush(stdout);
    }

    usleep(100000);
  }

  close(fd);
  close(user_fd);
  close(root_fd);
  close(user_log_fd);
  close(root_log_fd);
}

int main() {
  printf("╔════════════════════════════════════╗\n");
  printf("║   멀티 프로세스 명령어 실행 시스템   ║\n");
  printf("╚════════════════════════════════════╝\n");

  if(getuid() != 0)
  {
    fprintf(stderr, "\n 에러: 이 프로그램은 root 권한이 필요합니다!\n");
    fprintf(stderr, "\n사용법:\n");
    fprintf(stderr, "  sudo ./app 파일\n\n");
    return 1;
  }

  printf("\n root 권한 확인 (UID: %d)\n", getuid());

  signal(SIGINT, signal_handler);   // Ctrl+C
  signal(SIGTERM, signal_handler);  // kill 명령

  int user_pipe[2];
  int root_pipe[2];
  int user_log_pipe[2];
  int root_log_pipe[2];

  if(pipe(user_pipe) == -1)
  {
    perror("user_pipe 생성 실패");
    return 1;
  }

  if(pipe(root_pipe) == -1)
  {
    perror("root_pipe 생성 실패");
    return 1;
  }

  if(pipe(user_log_pipe) == -1)
  {
    perror("root_pipe 생성 실패");
    return 1;
  }
  

  if(pipe(root_log_pipe) == -1)
  {
    perror("root_pipe 생성 실패");
    return 1;
  }
  
  
  printf("파이프 생성 완료\n");

  // ------------- 사용자($) 프로세스 생성 -------------
  pid_t user_pid = fork();

  if(user_pid == 0)
  {
    close(user_pipe[1]);
    close(root_pipe[0]);
    close(root_pipe[1]);
    close(user_log_pipe[0]);
    close(root_log_pipe[0]);
    close(root_log_pipe[1]);

    dup2(user_log_pipe[1], STDOUT_FILENO);
    dup2(user_log_pipe[1], STDERR_FILENO);
    close(user_log_pipe[1]);

    if(setgid(1000) < 0) 
    {
      perror("setgid 실패");
      exit(1);
    }
    if(setuid(1000) < 0) 
    {
      perror("setuid 실패");
      exit(1);
    }

    user_process_loop(user_pipe[0]);
    exit(0);
  }

  g_user_pid = user_pid;

  printf("$ 프로세스 생성 (PID: %d)\n", user_pid);

  // ------------- 루트(#) 프로세스 생성 -------------
  pid_t root_pid = fork();

  if(root_pid == 0)
  {
    close(root_pipe[1]);
    close(user_pipe[0]);
    close(user_pipe[1]);
    close(root_log_pipe[0]);
    close(user_log_pipe[0]);
    close(root_log_pipe[1]);

    dup2(root_log_pipe[1], STDOUT_FILENO);
    dup2(root_log_pipe[1], STDERR_FILENO);
    close(root_log_pipe[1]);

    root_process_loop(root_pipe[0]);
    exit(0);
  }

  g_root_pid = root_pid;
  printf("# 프로세스 생성 (PID: %d)\n", root_pid);


  close(user_pipe[0]);
  close(root_pipe[0]);
  close(user_log_pipe[1]);
  close(root_log_pipe[1]);

  g_user_pipe_w = user_pipe[1];
  g_root_pipe_w = root_pipe[1];
  g_user_log_r = user_log_pipe[0];
  g_root_log_r = root_log_pipe[0];

  int flags = fcntl(user_log_pipe[0], F_GETFL, 0);
  fcntl(user_log_pipe[0], F_SETFL, flags | O_NONBLOCK);

  flags = fcntl(root_log_pipe[0], F_GETFL, 0);
  fcntl(root_log_pipe[0], F_SETFL, flags | O_NONBLOCK);

  printf("======================================\n");
  printf("테스트 명령어 실행 중입니다\n");
  printf("======================================\n");

  distribute_commands(user_pipe[1], root_pipe[1], user_log_pipe[0], root_log_pipe[0]);

  int status;
  waitpid(user_pid, &status, 0);
  waitpid(root_pid, &status, 0);

  return 0;
}
