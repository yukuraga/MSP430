/*
 * queue.c
 *
 *  Created on: 5 Eyl 2019
 *      Author: ugur
 */


#include"queue.h"
#include "stdint.h"

uint8_t last = 0;
static int8_t queue[QUEUE_SIZE] = {0};

void queue_push(char* val){
    if(last<QUEUE_SIZE){
        queue[last++] = *val;
    }
}

void queue_pop(){
    if(last){
        memmove(queue, (queue+1), last);
        last--;
    }
}

char* queue_front(){
    return queue;
}

int8_t queue_isEmpty(){
    if(last == 0)
        return TRUE;
    return FALSE;
}
