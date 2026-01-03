#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct{
    char type;
    char *command;
} CommandNode;

int execute_S_node(CommandNode *node)
{
    printf("\n[S노드] 실행 : %s\n", node->command);
    printf("[S노드] 명령어 완료 대기...\n");

    pid_t pid = fork();

    if(pid == 0)
    {
        execlp("bash", "bash", "-c", node->command, NULL);
        perror("exec 실패");
        exit(1);
    }
    else if(pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);

        if(WIFEXITED(status) && WEXITSTATUS(status) == 0)
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
    printf("[C노드] 명령어 완료 대기...\n");

    pid_t pid = fork();

    if(pid == 0)
    {
        pid_t child_pid = fork();

        if(child_pid == 0)
        {
            printf("[C노드] PID %d로 백그라운드 실행\n", getpid());
            execlp("bash", "bash", "-c", node->command, NULL);
            perror("exec 실패");
            exit(1);
        }
        else if(child_pid > 0)
        {
            exit(0);
        }
        else
        {
            printf("자식 fork 실패");
            exit(1);
        }
    }
    else if(pid > 0)
    {
        // 부모: 자식만 대기
        int status;
        waitpid(pid, &status, 0);
        
        printf("[C노드] ✓ 백그라운드 실행 시작 (대기 안 함)\n");
        
        return 0;  // exit(0) 아님!
    }
    else
    {
        perror("fork 실패");
        return -1;
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
        sleep(1);
    }   

    printf("\n======================================\n");
    printf("  모든 노드 처리 완료\n");
    printf("======================================\n");
    
    printf("\nC노드 프로세스는 백그라운드에서 계속 실행 중\n");
    printf("확인: ps aux | grep while\n");
    printf("확인: ps aux | grep ping\n");
    printf("메인 프로세스 종료는 Ctrl+c\n");

    while(1)
    {
        pause();
    }
    
    return 0;
}
