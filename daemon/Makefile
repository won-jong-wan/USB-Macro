# 컴파일러 및 플래그 설정
CC = gcc
CFLAGS = -Wall -g -Iheader

# 디렉토리 경로 변수 설정
SRC_DIR = source
OBJ_DIR = build
BIN_DIR = build

# 실행 파일 이름
TARGET = $(BIN_DIR)/sim
# ★ 중요: udev 라이브러리 링크
LIBS = -ludev 

# [수정됨] 소스 파일 목록 교체
# usb_sim.c 삭제 -> monitor_udev.c, driver_stm32.c 추가
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/executor.c \
       $(SRC_DIR)/monitor_udev.c \
       $(SRC_DIR)/driver_stm32.c

# 오브젝트 파일 목록 (자동 변환)
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

# [수정됨] 헤더 의존성 명시 (선택 사항이지만 안전을 위해 갱신)
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c header/monitor.h header/hw_driver.h header/executor.h header/common.h
$(OBJ_DIR)/executor.o: $(SRC_DIR)/executor.c header/executor.h
$(OBJ_DIR)/monitor_udev.o: $(SRC_DIR)/monitor_udev.c header/monitor.h
$(OBJ_DIR)/driver_stm32.o: $(SRC_DIR)/driver_stm32.c header/hw_driver.h header/common.h

# 청소
clean:
	rm -rf $(OBJ_DIR)