/*
 * msc.h
 *
 *  Created on: Dec 7, 2025
 *      Author: jonwo
 */

#ifndef INC_MSC_H_
#define INC_MSC_H_

#define DISK_BLOCK_NUM   32
#define DISK_BLOCK_SIZE  512
#include "main.h"

// 실제 디스크 역할을 할 메모리 배열
extern uint8_t msc_disk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE];

void init_disk_data(void);
void check_usb_file_command(void);

#endif /* INC_MSC_H_ */
