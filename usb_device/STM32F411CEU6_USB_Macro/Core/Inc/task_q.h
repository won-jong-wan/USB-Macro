/*
 * task_q.h
 *
 *  Created on: Dec 23, 2025
 *      Author: jonwo
 */

#ifndef INC_TASK_Q_H_
#define INC_TASK_Q_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // for NULL

typedef void (*task_t)(void);

#define MAX_QUEUE_SIZE 20

typedef struct {
	task_t buffer[MAX_QUEUE_SIZE];
	volatile uint8_t head;
	volatile uint8_t tail;
	volatile uint8_t count;
} task_q_t;

// 큐 초기화
static inline void q_init(task_q_t *q) {
	q->head = q->tail = q->count = 0;
}

static inline bool q_push(task_q_t *q, task_t task) {
	if (q->count >= MAX_QUEUE_SIZE)
		return false;

	q->buffer[q->tail] = task;
	q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
	q->count++;
	return true;
}

static inline task_t q_pop(task_q_t *q) {
	if (q->count == 0)
		return NULL;

	task_t task = q->buffer[q->head];
	q->head = (q->head + 1) % MAX_QUEUE_SIZE;
	q->count--;
	return task;
}

#endif /* INC_TASK_Q_H_ */
