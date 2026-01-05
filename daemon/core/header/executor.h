#ifndef EXECUTOR_H
#define EXECUTOR_H

int init_executor(void);
// 기존 함수
int execute_command(const char *cmd, int is_background);
// [신규] 실행 중인 백그라운드 작업 모두 종료
void terminate_background_jobs(void);

#endif