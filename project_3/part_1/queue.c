#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"


void initializeQueue(Queue* q)
{
    q->front = -1;
    q->rear = 0;
}


int isEmpty(Queue* q) { return (q->front == q->rear - 1); }


int isFull(Queue* q) { return (q->rear == MAX_SIZE); }


void enqueue(Queue* q, int value)
{
    if (isFull(q)) {
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}


int dequeue(Queue* q)
{
    if (isEmpty(q)) {
        return -1;
    }
    int ret = q->items[q->front+1];
    q->front++;
    return ret;
}


int peek(Queue *q) {
    if (isEmpty(q)) {
        return -1;
    }
    return q->items[q->front + 1];
}


void empty_queue(Queue* q)
{
    while(!isEmpty(q))
    {
        dequeue(q);
    }
}

