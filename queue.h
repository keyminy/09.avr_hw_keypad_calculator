/*
 * queue.h
 *
 * Created: 2021-09-23 오전 10:25:21
 *  Author: kcci
 */ 


#ifndef QUEUE_H_
#define QUEUE_H_

#define TRUE 1
#define FALSE 0
#define QUEUE_MAX 100

int queue_full(void);
int queue_empty();
uint8_t read_queue();
void queue_init();
void insert_queue(uint8_t value);


#endif /* QUEUE_H_ */