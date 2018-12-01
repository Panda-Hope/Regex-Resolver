
//
//  automat.c
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/19.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#include "automat.h"

/* ========================================
 *      自动机数据结构相关操作函数
 * ======================================== */

Automat *InitAutomat(void)
{
    Automat *automat = malloc(sizeof(Automat));
    automat->f = automat->r = NULL;
    
    return automat;
}

Vertex *InitVertex(void)
{
    Vertex *v = malloc(sizeof(Vertex));
    v->arc = NULL;
    
    return v;
}

Arc *InitArc(void)
{
    Arc *arc = malloc(sizeof(Arc));
    arc->next = NULL;
    arc->fn = NULL;
    arc->c = 0;
    arc->v = NULL;
    
    return arc;
}

void FreeVertex(Vertex *v)
{
    free(v);
    v = NULL;
}

void FreeArc(Arc *arc)
{
    free(arc);
    arc = NULL;
}

void FreeAutomat(Automat *a)
{
    free(a);
    a = NULL;
}

/* 形成闭环了，释放不了o(╯□╰)o */
void FreeRegex(Vertex *v)
{
//    if (!v || !v->arc) return;
//
//    Arc *arc = v->arc;
//    Arc *t = NULL;
//
//    while (arc) {
//        v = arc->v;
//        t = arc;
//        arc = arc->next;
//
//        t->v = NULL;
//        FreeArc(t);
//        FreeRegex(v);
//    }
//    FreeVertex(v);
}

/*
 * 为任意的符号C创建一个弧为C的自动机, 当c=0时意味着空转换弧
 * */
Automat *CreateArc(char c)
{
    Vertex *r = InitVertex();
    
    Arc *arc = InitArc();
    arc->c = c;
    arc->v = r;
    
    Vertex *f = InitVertex();
    f->arc = arc;
    
    Automat *automat = InitAutomat();
    automat->f = f;
    automat->r = r;
    
    return automat;
}

Automat *CreateDataArc(bool (*fn) (char))
{
    Vertex *r = InitVertex();
    
    Arc *arc = InitArc();
    arc->fn = fn;
    arc->v = r;
    
    Vertex *f = InitVertex();
    f->arc = arc;
    
    Automat *automat = InitAutomat();
    automat->f = f;
    automat->r = r;
    
    return automat;
}

/* ========================================
 *      正则表达式基本数据类型及运算操作
 *   数据类型:
 *      . : 匹配任意一个非换行符符号
 *      \d: 匹配0~9的数字
 *      \t: 匹配制表符
 *      \s: 匹配一个空白字符，包括空格、制表符、换页符和换行符
 *      \w: 匹配一个单字字符（字母、数字或者下划线）等价于[A-Za-z0-9_]
 *   运算类型:
 *     (a): 创建一个子正则表达式a
 *     a* : 匹配多次或零次
 *     a|b: 匹配a或者b
 * ======================================== */

bool NonLineBreak(char c)
{
    return c != '\n';
}

bool IsDigit(char c)
{
    return isdigit((int)c) != 0;
}

bool MatchTab(char c)
{
    return c == '\t';
}

bool MatchWhiteSpace(char c)
{
    switch (c) {
        case ' ':
        case '\t':
        case '\n':
        case '\f':
            return true;
        default:
            return false;
    }
}

bool MatchSingle(char c)
{
    return isalnum((int)c) != 0;
}

/*
 * 将自动机a与自动机b串联
 * */
Automat *Link(Automat *a, Automat *b)
{
    a->r->arc = b->f->arc;
    a->r = b->r;
    FreeVertex(b->f);
    FreeAutomat(b);
    
    return a;
}

/*
 * 将自动机a与自动机b并联
 * */
Automat *Parallel(Automat *a, Automat *b)
{
    a->f->arc->next = b->f->arc;
    FreeVertex(b->f);
    FreeAutomat(b);
    
    return a;
}

/*
 * 为自动机A执行幂运算
 * */
Automat *Multiple(Automat *a)
{
    /* 为自动机A构造一条闭合回溯路径 */
    Arc *closure = InitArc();
    closure->c = 0;
    closure->v = a->f;
    a->r->arc = closure;
    
    Automat *start = CreateArc(0);
    start->f->arc->v = a->f;
    FreeVertex(start->r);
    start->r = a->r;
    
    Automat *rear = CreateArc(0);
    start->r->arc->next = rear->f->arc;
    start->r = rear->r;
    FreeVertex(rear->f);
    
    Arc *parallel = InitArc();
    parallel->c = 0;
    parallel->v = start->r;
    start->f->arc->next = parallel;
    
    return start;
}

/*
 * 为自动机A与自动机B执行并运算
 * */
Automat *Or(Automat *a, Automat *b)
{
    Automat *top = CreateArc(0);
    Automat *bottom = CreateArc(0);
    
    Link(top, a);
    Link(bottom, b);
    
    Parallel(top, bottom);
    
    Automat *temp = CreateArc(0);
    Link(top, temp);
    
    Arc *rear = InitArc();
    rear->c = 0;
    rear->v = top->r;
    bottom->r->arc = rear;
    
    return top;
}

/*
 * 为每一个称之为NFA自动机状态的顶点T求其Closure(T)集合
 * Closure(T): NFA自动机状态T及其可以通过空转换可以达到的状态的集合
 * */
Stack *StateClosure(const Vertex *t)
{
    Stack *State_Stack = InitStack();
    Stack *Closure_Stack = InitStack();
    
    Push(State_Stack, t);                               // 从状态T开始进行闭包查询
    Push(Closure_Stack, t);                             // 包含初始项T
    
    /* 对状态T进行递归栈查询空状态集合 */
    while(!EmptyStack(State_Stack)) {
        Vertex *v = Pop(State_Stack);
        Arc *arc = v->arc;
        
        while (arc) {
            if (arc->c == 0 && !arc->fn) {
                Push(Closure_Stack, arc->v);
                Push(State_Stack, arc->v);
            }
            
            arc = arc->next;                            // 对状态T的每一条弧进行闭包查询
        }
    }
    FreeStack(State_Stack);
    
    return Closure_Stack;
}

/* ========================================
 *          NFA自动机驱动程序相关函数
 * ======================================== */

/*
 * 对弧的基本数据进行校验
 * */
bool DataCheck(char c, Arc *arc)
{
    if (!arc) return false;                                     // 对终结状态直接返回FALSE
    
    return  arc->fn ? arc->fn(c) : arc->c == c;
}

/*
 * 正则表达式匹配函数，NFA自动机驱动程序
 * 对输入的字符进行正则匹配, 判断其是否属于正则表达式Regex所定义的语言集
 * text:  需要校验的字符串
 * state: NFA自动机的起始状态
 * */
bool AutomatDriver(const char *text, const Vertex *state)
{
    Stack *Closure_Stack = NULL;
    
    while (*text != '\0') {
        Closure_Stack = StateClosure(state);
        bool checked = false;
        
        /* 根据输入来使自动机走向下一个状态 */
        while(!EmptyStack(Closure_Stack)) {
            Vertex *v = Pop(Closure_Stack);
            
            if (DataCheck(*text, v->arc)) {
                checked = true;
                state = v->arc->v;
                text++;
                break;
            }
        }
        
        FreeStack(Closure_Stack);
        
        if (!checked) return false;
    }
    
    Stack *FinalState_Stack = StateClosure(state);
    
    /* 检测其最终的状态集合中是否包含终结状态 */
    while (!EmptyStack(FinalState_Stack)) {
        Vertex *v = Pop(FinalState_Stack);
        
        if (!v->arc) return true;                               // 将缺乏转换弧的状态定义为终结状态
    }
    
    FreeStack(FinalState_Stack);
    
    return false;
}
