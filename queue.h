//
//  queue.h
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/23.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#ifndef queue_h
#define queue_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Queue Queue;
typedef struct QNode QNode;

struct Queue {
    QNode *front;
    QNode *rear;
};

struct QNode {
    const void *data;
    QNode *next;
};

Queue *InitQueue(void);

QNode *InitQNode(void);

void EnQueue(Queue *q, const void *ele);

void *DeQueue(Queue *q);

void *GetHead(const Queue *q);

bool EmptyQueue(const Queue *q);

void QueueError(void);

void QueueEmptyError(void);

void FreeQueue(Queue *q);

#endif /* queue_h */
