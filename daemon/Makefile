# 컴파일러 및 플래그 설정
CC = gcc
# [수정됨] 헤더 파일 포함 경로 변경 (header -> core/header)
CFLAGS = -Wall -g -Icore/header

# [수정됨] 소스 디렉토리 경로 변경 (source -> core/source)
SRC_DIR = core/source
OBJ_DIR = build
BIN_DIR = build

# 실행 파일 이름
TARGET = $(BIN_DIR)/sim
# ★ 중요: udev 라이브러리 링크
LIBS = -ludev 

# 소스 파일 목록
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/executor.c \
       $(SRC_DIR)/monitor_udev.c \
       $(SRC_DIR)/driver_stm32.c

# 오브젝트 파일 목록 (자동 변환: core/source/%.c -> build/%.o)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# 기본 타겟
all: create_dir $(TARGET)

# 빌드 디렉토리 생성
create_dir:
	@mkdir -p $(OBJ_DIR)

# 실행 파일 링크
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# 컴파일 규칙 (Pattern Rule)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# [수정됨] 헤더 의존성 경로 업데이트 (header/ -> core/header/)
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c core/header/monitor.h core/header/hw_driver.h core/header/executor.h core/header/common.h
$(OBJ_DIR)/executor.o: $(SRC_DIR)/executor.c core/header/executor.h
$(OBJ_DIR)/monitor_udev.o: $(SRC_DIR)/monitor_udev.c core/header/monitor.h
$(OBJ_DIR)/driver_stm32.o: $(SRC_DIR)/driver_stm32.c core/header/hw_driver.h core/header/common.h

# 청소
clean:
	rm -rf $(OBJ_DIR)