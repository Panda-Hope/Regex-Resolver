//
//  queue.c
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/23.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#include "queue.h"

Queue *InitQueue(void)
{
    Queue *queue = malloc(sizeof(Queue));
    
    if (!queue) QueueError();
    queue->front = queue->rear = InitQNode();
    
    return queue;
}

QNode *InitQNode(void)
{
    QNode *node = malloc(sizeof(QNode));
    
    if (!node) QueueError();
    node->data = NULL;
    node->next = NULL;
    
    return node;
}

void EnQueue(Queue *q, const void *ele)
{
    q->rear->next = InitQNode();
    q->rear = q->rear->next;
    q->rear->data = ele;
}

void *DeQueue(Queue *q)
{
    if (q->front == q->rear) QueueEmptyError();
    
    QNode *t = q->front->next;
    void *data = (void *)t->data;
    
    q->front->next = t->next;
    if (!t->next) q->rear = q->front;
    
    free(t);
    t = NULL;
    
    return data;
}

void *GetHead(const Queue *q)
{
    if (q->front == q->rear) QueueEmptyError();
    
    void *data = (void *)q->front->next->data;
    return data;
}

bool EmptyQueue(const Queue *q)
{
    return q->front == q->rear;
}

void QueueError(void)
{
    printf("队列内存分配失败\n");
    exit(1);
}

void QueueEmptyError(void)
{
    printf("队列已空\n");
    exit(1);
}

void FreeQueue(Queue *q)
{
    QNode *node = q->front;
    
    while (node) {
        QNode *t = node;
        free(t);
        t = NULL;
        node = node->next;
    }
    
    free(q);
    q = NULL;
}
