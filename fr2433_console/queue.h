/*
 * queue.h
 *
 *  Created on: 5 Eyl 2019
 *      Author: ugur
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#define TRUE (1)
#define FALSE (0)

#define QUEUE_SIZE 8

void queue_push(char* val);
void queue_pop();
char* queue_front();

#endif /* QUEUE_H_ */
