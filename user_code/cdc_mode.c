#include "define.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <termios.h>  // ✅ 추가!

#define MAX_CMD_LENGTH 512

int check_vid_pid(const char *device, const char *target_vid, const char *target_pid)
{
    char cmd[256];
    snprintf(cmd, sizeof(cmd), 
             "udevadm info -n %s | grep -E 'ID_VENDOR_ID=%s|ID_MODEL_ID=%s' | wc -l",
             device, target_vid, target_pid);
    
    FILE *fp = popen(cmd, "r");
    if(!fp) return 0;
    
    int count = 0;
    fscanf(fp, "%d", &count);
    pclose(fp);
    
    return (count >= 2);
}

int find_cdc_device(char *result_path, size_t size)
{
    DIR *dir = opendir("/sys/class/tty");
    if(!dir)
    {
        perror("opendir failed");
        return 0;
    }

    struct dirent *entry;

    while((entry = readdir(dir)) != NULL)
    {
        if(strncmp(entry->d_name, "ttyACM", 6) != 0)
        {
            continue;
        }
        
        char dev_path[64];
        snprintf(dev_path, sizeof(dev_path), "/dev/%s", entry->d_name);
        
        printf("[확인] %s...", dev_path);
        
        if(check_vid_pid(dev_path, "cafe", "dead"))
        {
            snprintf(result_path, size, "%s", dev_path);
            printf(" ✓ 팀 CDC 디바이스 발견! (VID:PID = cafe:dead)\n");
            closedir(dir);
            return 1;
        }
        
        printf(" (다른 디바이스)\n");
    }
    
    closedir(dir);
    printf("\n[실패] cafe:dead CDC 디바이스를 찾을 수 없음\n");
    return 0;
}

void execute_and_send(int fd, const char *command)
{
    printf("\n[CDC] 명령어 실행: %s\n", command);
    printf("─────────────────────────────────────\n");

    FILE *fp = popen(command, "r");
    if(fp == NULL)
    {
        perror("popen 실패");
        const char *error_msg = "[ERROR] Command execution failed\n";
        write(fd, error_msg, strlen(error_msg));
        
        // ✅ 오류 후 버퍼 비우기
        tcdrain(fd);
        usleep(50000);
        tcflush(fd, TCIFLUSH);
        
        return;
    }

    char buffer[1024];
    while(fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        printf("%s", buffer);
        fflush(stdout);
        write(fd, buffer, strlen(buffer));
    }

    int status = pclose(fp);
    printf("─────────────────────────────────────\n");

    if(status == -1)
    {
        perror("pclose 실패");
        const char *error_msg = "[ERROR] Command completion failed\n";
        write(fd, error_msg, strlen(error_msg));
    }
    else if(WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        printf("[CDC] 완료 (종료 코드: %d)\n\n", exit_code);

        char done_msg[100];
        snprintf(done_msg, sizeof(done_msg), "\n[DONE] Exit code : %d\n\n", exit_code);
        write(fd, done_msg, strlen(done_msg));
    }
    else
    {
        printf("[CDC] 실패\n\n");
        const char *fail_msg = "\n[Failed]\n";
        write(fd, fail_msg, strlen(fail_msg));
    }

    // ✅ 명령어 완료 후 버퍼 비우기 (루프백 방지!)
    tcdrain(fd);           // 전송 완료 대기
    usleep(100000);        // 100ms 대기
    tcflush(fd, TCIFLUSH); // 입력 버퍼 비우기
    
    printf("[DEBUG] 입력 버퍼 비움\n");  // 디버깅용
}

void cdc_mode_loop(int fd)
{
    printf("\n════════════════════════════════════\n");
    printf("║        CDC 모드 시작              ║\n");
    printf("════════════════════════════════════\n");
    printf("UART 문자열 명령어 대기 중...\n\n");

    char cmd_buffer[MAX_CMD_LENGTH];
    int cmd_pos = 0;
    int command_count = 0;

    // ✅ 시작 전 버퍼 비우기
    tcflush(fd, TCIOFLUSH);

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    while(1)
    {
        char read_buffer[64];
        ssize_t n = read(fd, read_buffer, sizeof(read_buffer));

        if(n < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                usleep(10000);
                continue;
            }
            perror("[CDC] 읽기 실패");
            break;
        }

        if(n == 0)
        {
            usleep(10000);
            continue;
        }

        for(ssize_t i = 0; i < n; i++)
        {
            char ch = read_buffer[i];

            if(ch == '\n' || ch == '\r')
            {
                if(cmd_pos == 0)
                {
                    continue;
                }

                cmd_buffer[cmd_pos] = '\0';
                
                int is_valid = 0;
                
                if(strchr(cmd_buffer, ' ') != NULL)
                {
                    is_valid = 1;
                }
                else if(cmd_pos <= 10)
                {
                    is_valid = 1;
                }
                else if(strstr(cmd_buffer, ".c") || 
                        strstr(cmd_buffer, ".o") || 
                        strstr(cmd_buffer, ".ko") ||
                        strstr(cmd_buffer, "_"))
                {
                    printf("[무시] 파일명 같음: %s\n", cmd_buffer);
                    cmd_pos = 0;
                    continue;
                }
                
                if(!is_valid)
                {
                    printf("[무시] 유효하지 않은 명령어: %s\n", cmd_buffer);
                    cmd_pos = 0;
                    continue;
                }
                
                command_count++;
                printf("[CDC] 명령어 #%d 수신: %s\n", command_count, cmd_buffer);

                if(strcmp(cmd_buffer, "exit") == 0 || strcmp(cmd_buffer, "quit") == 0)
                {
                    printf("\n[CDC] 종료 명령어 수신\n");
                    const char *bye_msg = "\n[CDC] GoodBye!!\n";
                    write(fd, bye_msg, strlen(bye_msg));
                    goto loop_exit;
                }

                execute_and_send(fd, cmd_buffer);
                cmd_pos = 0;
            }
            else
            {
                if(cmd_pos < MAX_CMD_LENGTH - 1)
                {
                    cmd_buffer[cmd_pos++] = ch;
                }
                else
                {
                    printf("[CDC] 명령어 너무 김 (최대 %d자)\n", MAX_CMD_LENGTH);
                    cmd_pos = 0;
                }
            }
        }
    }

loop_exit:
    printf("\n════════════════════════════════════\n");
    printf("║      CDC 모드 종료                ║\n");
    printf("║   총 %d개 명령어 실행              ║\n", command_count);
    printf("════════════════════════════════════\n");
}

int main()
{
    printf("╔════════════════════════════════════╗\n");
    printf("║      (Emergency) CDC 모드          ║\n");
    printf("╚════════════════════════════════════╝\n");

    printf("\n✓ 사용자: %s (UID: %d)\n", getuid() == 0 ? "root" : "일반 사용자", getuid());

    char dev_path[32];
    int fd;

    printf("\nVID:PID = cafe:dead 디바이스 검색 중...\n");
    
    if(!find_cdc_device(dev_path, sizeof(dev_path)))
    {
        printf("\n✗ 팀 CDC 디바이스를 찾을 수 없습니다.\n");
        printf("  연결된 CDC 장치 목록:\n");
        system("ls -l /dev/ttyACM* 2>/dev/null || echo '  (없음)'");
        return 1;
    }

    printf("\n디바이스 오픈 시도: %s\n", dev_path);

    fd = open(dev_path, O_RDWR | O_NOCTTY);
    if(fd < 0)
    {
        perror("디바이스 오픈 실패");
        return 1;
    }

    printf("디바이스 오픈 성공 (fd: %d)\n", fd);

    cdc_mode_loop(fd);

    close(fd);
    printf("\n프로그램 종료\n");

    return 0;
}