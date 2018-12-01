//
//  stack.c
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/18.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#include "stack.h"

Stack *InitStack(void)
{
    Stack *s = malloc(sizeof(Stack));
    s->size = STACK_INIT_SIZE;
    s->top = s->base = calloc(sizeof(void *), STACK_INIT_SIZE);
    
    if (!s->base) StackError();
    
    return s;
}

void Push(Stack *s, const void *ele)
{
    if (s->top - s->base >= s->size) {
        s->base = realloc(s->base, s->size);
        if (!s->base) StackError();
        
        s->top = s->base + s->size;
        s->size += STACK_INCREMENT;
    }

    *(s->top++) = ele;
}

void *Pop(Stack *s)
{
    if (s->top <= s->base) StackEmpty();
    return (void*)*(--s->top);
}

void *GetTop(const Stack *s)
{
    if (s->base == s->top) StackEmpty();
    return (void*)*(s->top-1);
}

void StackError(void)
{
    printf("栈内存分配失败\n");
    exit(1);
}

void StackEmpty(void)
{
    printf("栈已空\n");
    exit(1);
}

void FreeStack(Stack *s)
{
    free(s->base);
    s->base = NULL;
    free(s);
    s = NULL;
}

size_t Stack_Size(Stack *s)
{
    return s->top - s->base;
}

bool EmptyStack(Stack *s)
{
    return s->base == s->top;
}
