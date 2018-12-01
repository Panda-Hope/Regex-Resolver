//
//  automat.h
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/19.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#ifndef automat_h
#define automat_h

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "stack.h"

/* ========================================
 *      自动机数据结构及相关操作函数
 * ======================================== */

typedef struct Automat Automat;
typedef struct Vertex Vertex;
typedef struct Arc Arc;

/*
 * 自动机数据结构
 * f 自动机入口
 * r 自动机出口
 * */
struct Automat {
    Vertex *f;
    Vertex *r;
};

/*
 * 自动机状态图顶点
 * a 第一条与顶点v相连的弧(即状态转换)
 * */
struct Vertex {
    Arc *arc;
};

/*
 * 自动机状态图转换弧
 * v    与弧相接的下一个顶点
 * next 与顶点相接的下一条弧
 * c    当前转换弧上的转换符号，当c=0时意味着空转换
 * fn   转态转换验证函数
 * */
struct Arc {
    Vertex *v;
    Arc *next;
    char c;
    bool (*fn) (char c);
};

Automat *InitAutomat(void);

Vertex *InitVertex(void);

Arc *InitArc(void);

void FreeVertex(Vertex *v);

void FreeArc(Arc *arc);

void FreeAutomat(Automat *a);

void FreeRegex(Vertex *v);

/* 为任意的符号C创建一个弧为C的自动机, 当c=0时意味着空转换弧 */
Automat *CreateArc(char c);

/* 为一个数据类型建立相应的数据类型检查 */
Automat *CreateDataArc(bool (*fn) (char));


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

bool NonLineBreak(char c);

bool IsDigit(char c);

bool MatchTab(char c);

bool MatchWhiteSpace(char c);

bool MatchSingle(char c);

/* 将自动机a与自动机b连接 */
Automat *Link(Automat *a, Automat *b);

/* 将自动机a与自动机b并联 */
Automat *Parallel(Automat *a, Automat *b);

/* 为自动机A执行幂运算 */
Automat *Multiple(Automat *a);

/* 为自动机A与自动机B执行并运算 */
Automat *Or(Automat *a, Automat *b);

/*
 * 为每一个称之为NFA自动机状态的顶点T求其Closure(T)集合
 * Closure(T): NFA自动机状态T及其可以通过空转换可以达到的状态的集合
 * */
Stack *StateClosure(const Vertex *t);


/* ========================================
 *          NFA自动机驱动程序相关函数
 * ======================================== */

/* 对弧的基本数据进行校验 */
bool DataCheck(const char c, Arc *arc);

/*
 * 正则表达式匹配函数，NFA自动机驱动程序
 * 对输入的字符进行正则匹配判断其是否属于正则表达式Regex所定义的语言集
 * text:  需要校验的字符串
 * state: NFA自动机的起始状态
 * */
bool AutomatDriver(const char *text, const Vertex *state);

#endif /* automat_h */
