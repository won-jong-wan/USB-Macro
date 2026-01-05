#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

// MAX_C_NODES : 최대 실행시킬 수 있는 프로세스 개수
// BUFFER_SIZE : 로그를 한 번에 읽을 수 있는 길이(넘어가면 크기 다 읽을 때까지 읽음)
#define MAX_C_NODES 10
#define BUFFER_SIZE 1024

typedef struct{
    char type;
    char *command;
} CommandNode;

typedef struct{
    pid_t pid;
    int pipe_fd;    // 파이프 읽기 파일 디스크립터(파이프 번호)
    char *command;
} BackgroundProcess;

BackgroundProcess bg_processes[MAX_C_NODES];
int bg_count = 0;

int execute_S_node(CommandNode *node)
{
    printf("\n[S노드] 실행 : %s\n", node->command);
    printf("[S노드] 명령어 완료 대기...\n");

    pid_t pid = fork();
    // pid_t는 프로세스 ID를 담는다는 것을 표현하기 위함
    // unistd.h에 포함

    if(pid == 0)
    {
        execlp("bash", "bash", "-c", node->command, NULL);
        // execlp의 p는 PATH를 의미 → PATH에서 자동으로 찾음
        // 첫 번째 bash : PATH 환경변수에서 "bash" 프로그램 찾기
        // 두 번째 bash : 그냥 이름임
        // -c : bash 옵션(쉘 명령어 실행)
        // node->command : 실행시킬 명령어

        perror("exec 실패");
        exit(1);
    }
    else if(pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);

        if(WIFEXITED(status) && WEXITSTATUS(status) == 0)
        // WIFEXITED(status) : 정상 종료를 했는가
        // WEXITSTATUS(status) : 종료 코드가 어떤 것인지 
        {
            printf("[S노드] 완료\n");
            return 0;
        }
        else
        {
            printf("[S노드] 실패\n");
            return 1;
        }
    }
    else
    {
        perror("fork 실패");
        return -1;
    }
}

int execute_C_node(CommandNode *node)
{
    printf("\n[C노드] 실행 : %s\n", node->command);
   
    if(bg_count >= MAX_C_NODES)
    {
        printf("[C노드] 최대 백그라운드 프로세스 개수 초과\n");
        return -1;
    }

    // 파이프 파일 디스크립터 2개를 담을 배열 생성
    // 파이프 생성
    int pipefd[2];
    if(pipe(pipefd) == -1) // 파이프를 생성하고 제대로 생성됐는지 확인
    {
        perror("pipe 생성 실패");
        return -1;
    }

    pid_t pid = fork();

    if(pid == 0)
    {
        // 파일 디스크립터를 닫음(읽기용)
        // 자식은 명령어 쓰기만 하니까 읽기가 필요 없음
        close(pipefd[0]);   

        // dup2 : duplicate file descriptor 2(파일 디스크립터 복제 함수)
        // STDOUT_FILENO : 일반적인 모든 출력
        // STDERR_FILENO : 에러/경고 메시지
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FISTDOUT_FILENOLENO);
        // 파이프로 출력 방향을 전환시킴
        
        close(pipefd[0]);

        execlp("bash", "bash", "-c", node->command, NULL);
        perror("exec 실패");
        exit(1);
    }
    else if(pid > 0)
    {
        close(pipefd[1]);   // 쓰기용 파이프 닫아줌. 부모(메인)는 읽어서 출력만 하니까
        
        int flags = fcntl(pipefd[0], F_GETFL, 0);
        // fcntl : File CoNToL (파일 제어 함수)
        // 파일 디스크립터의 속성을 읽거나 변경
        // F_GETFL : 모든 옵션을 가져옴
        fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
        // F_SETFL : 플래그를 설정
        // 기존 설정과 함께 논블로킹을 설정해줌
        // 논블로킹 이유 : C노드 명령어가 끝나는 것을 기다리지 않고 다른 명령어를 진행시키기 위함

        // 출력을 위해서 정보를 저장
        bg_processes[bg_count].pid = pid;
        bg_processes[bg_count].pipe_fd = pipefd[0];
        bg_processes[bg_count].command = node->command;
        bg_count++;
        
        printf("[C노드] ✓ 백그라운드 실행 시작 (대기 안 함)\n");
        
        return 0;  // exit(0) 아님!
    }
    else
    {
        perror("fork 실패");
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }
}

// 백그라운드 프로세스의 출력을 읽기 위함
void read_background_output()
{
    char buffer[BUFFER_SIZE];

    // 생성된 프로세스만큼 반복
    for(int i = 0; i < bg_count; i++)
    {
        ssize_t byte_read;

        // 데이터를 읽지 못했을 때 까지, 마지막은 널문자~
        while((byte_read = read(bg_processes[i].pipe_fd, buffer, BUFFER_SIZE-1)) > 0)
        {
            buffer[byte_read] = '\0';
            printf("[백그라운드 PID %d] %s", bg_processes[i].pid, buffer);
            fflush(stdout);
            // stdout로 출력을 해주고 fflush로 버퍼를 비워줌
            // 실시간으로 로그가 생길 때마다 출력
        }

        // 종료됐을 때(정상 or 에러)
        if(byte_read == 0 || (byte_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK))
        {
            if(byte_read == 0)
            {
                printf("[백그라운드 PID %d] 프로세스 종료\n", bg_processes[i].pid);
            }
            close(bg_processes[i].pipe_fd);
            bg_processes[i].pipe_fd = -1;
        }
    }
}

int main()
{
    printf("======================================\n");
    printf("테스트 명령어 실행 중입니다\n");
    printf("======================================\n");

    CommandNode nodes[] = {
        {'S', "sudo apt update"},
        {'C', "ping localhost"},
        {'S', "sudo apt upgrade"},
        {'C', "while true; do date; sleep 1; done"}
    };

    int node_count = sizeof(nodes) / sizeof(nodes[0]);

    printf("총 %d개 노드:\n", node_count);
    for(int i=0; i<node_count; i++)
    {
        printf("[노드 %d] 타입 : %c, 명령 : %s\n",
                i+1, nodes[i].type, nodes[i].command);
    }

    printf("\n실행시작...\n");

    for(int i=0; i<node_count; i++)
    {
        printf("\n======================================\n");
        printf("노드 %d/%d 처리\n", i + 1, node_count);
        printf("========================================\n");

        if(nodes[i].type == 'S')
        {
            int result = execute_S_node(&nodes[i]);

            if(result != 0)
            {
                printf("\nS노드 실패, 중단하겠습니다.");
                break;
            }
        }
        else if(nodes[i].type == 'C')
        {
            execute_C_node(&nodes[i]);
        }
        else
        {
            printf("\n알 수 없는 노드 %c\n", nodes[i].type);
        }
        read_background_output();
        sleep(1);
    }   

    printf("\n======================================\n");
    printf("  모든 노드 처리 완료\n");
    printf("======================================\n");
    
    printf("\nC노드 프로세스는 백그라운드에서 계속 실행 중\n");
    printf("확인: ps aux | grep while\n");
    printf("확인: ps aux | grep ping\n");
    for(int i=0; i<bg_count; i++)
    {
        printf("  - PID %d: %s\n", bg_processes[i].pid, bg_processes[i].command);
    }
    printf("메인 프로세스 종료는 Ctrl+c\n");

    while(1)
    {
        read_background_output();
        usleep(100000);
    }
    
    return 0;
}

