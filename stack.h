//
//  stack.h
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/18.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#ifndef stack_h
#define stack_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STACK_INIT_SIZE 20
#define STACK_INCREMENT 20

typedef struct Stack Stack;

struct Stack {
    const void **top;
    const void **base;
    int size;
};

Stack *InitStack(void);

void Push(Stack *s, const void *ele);

void *Pop(Stack *s);

void *GetTop(const Stack *s);

void StackError(void);

void StackEmpty(void);

void FreeStack(Stack *s);

size_t Stack_Size(Stack *s);

bool EmptyStack(Stack *s);

#endif /* stack_h */
