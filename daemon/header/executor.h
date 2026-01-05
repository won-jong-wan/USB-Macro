#ifndef EXECUTOR_H
#define EXECUTOR_H

// 초기화: Root 쉘과 User 쉘을 띄우고 준비 (성공 0, 실패 -1)
int init_executor();

// 명령어 실행 인터페이스 (main.c 호환용)
// cmd: 명령어 문자열
// is_background: 1(C-Node), 0(S-Node)
int execute_command(const char *cmd, int is_background);

#endif