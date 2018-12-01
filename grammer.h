//
//  grammer.h
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/16.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#ifndef grammer_h
#define grammer_h

#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "queue.h"
#include "automat.h"

/* ========================================
 *       正则语法DFA自动机驱动程序相关数据结构
 * ======================================== */

typedef struct LexeItem LexeItem;
typedef struct Grammer Grammer;
typedef struct ItemsArc ItemsArc;
typedef struct SetOfItems SetOfItems;

/*
 * 词法数据结构
 * type: 正则表达式符号类型，数据类型为i, 运算符类型为其对应符号例如 *、|、(、)
 * data: 数据类型:i 所包含的实际数值
 */
struct LexeItem {
    char type;
    char data;
    bool (*fn) (char);
};

/*
 * 文法数据结构
 * 对每一个形如: A->α 的文法构建一个DFA自动机, 并将自动机的每一个状态节点Vertex 称之为此文法产生式的一个"项"
 * */
struct Grammer {
    char header;
    const char *body;
    int size;
    int count;                                         // 文法产生式编号
    Automat *automat;                                  // 文法产生式自动机
};

/* 语法分析表自动机连接弧  */
struct ItemsArc {
    char c;
    ItemsArc *next;
    SetOfItems *items;
};

/*
 * 项集的闭包数据结构, 将每一个项集的闭包称之为语法分析表自动机的一个状态
 * SetOfItem_Stack: 项集中所包含的项的集合
 * arc: 当前项集状态可以通过输入而转换到其他状态的弧
 */
struct SetOfItems {
    Stack *SetOfItem_Stack;
    ItemsArc *arc;
};

/* ========================================
 *          正则表达式文法数据结构
 * ======================================== */

/*
 * 正则表达式语法分析程序
 * 对于目前正则表达式解析器构造如下的文法:
 * Q -> S           对文法S的增强文法
 * S -> S or T | T (用字母or替代|避免与文法分隔符"|"冲突)
 * T -> TE | E
 * E -> E* | F
 * F -> id | (S)
 * */

/* 正则表达式文法, 文法的出现顺序决定了文法的优先级 */
static Grammer grammers[9] = {
    {'Q', "S", 1, 0, NULL},
    {'S', "S|T", 3, 1, NULL},
    {'S', "T", 1, 2, NULL},
    {'T', "TE", 2, 3, NULL},
    {'T', "E", 1, 4, NULL},
    {'E', "E*", 2, 5, NULL},
    {'E', "F", 1, 6, NULL},
    {'F', "i", 1, 7, NULL},
    {'F', "(S)", 3, 8, NULL}
};
static const int grammerSize = sizeof(grammers)/sizeof(grammers[0]);
static const char grammerCharacters[10] = {'Q', 'S', '|', 'T', 'E', '*', 'F', 'i', '(', ')'};  // 文法S中的文法符号集合
static Stack *GrammerTable_Stack = NULL;        // 为文法S构造的语法分析自动机程序, 栈中每一个项为一个闭包项集状态


/* ========================================
 *        正则表达式文法分析表构造相关算法
 * ======================================== */

ItemsArc *InitItemsArc(void);

/* 初始化语法分析自动机状态数据 */
SetOfItems *InitSetOfItems(void);

/* 为每一个形如: A->α 的文法产生式构造一个DFA自动机 */
void CreateGrammerAutomat(Grammer *g);

void InitGrammerAutomat(void);

/*
 * 对某个语法状态求其闭包即对其中的“项”求与之其相等的"项"的集合
 * items: 语法自动机中的一个闭包集
 */
void GetClosureSetOfItems(SetOfItems *items);

/* 返回某个闭包状态在输入文法符号C之后得到后继闭包集 */
SetOfItems *Goto(SetOfItems *current, char c);

/* 检测两个状态闭包集是否相等 */
bool CheckItemsEqual(SetOfItems *a, SetOfItems *b);

/*
 * 为上面所描述的文法构建一个正则表达式语法分析自动机
 * */
Stack *GetGrammerTable(void);

/* 词法分析程序，将正则表达式转变为词法单元队列 */
Queue *LexeAnalysis(const char *regex);

/* 为一个基本的正则表达式数据类型符号，构建一个基本的正则表达式自动机模型 */
void BasicAutomat(Stack *Automat_Stack, LexeItem *lexeItem);

/* 为一个子正则表达式执行幂运算 */
void MutipleAutomat(Stack *Automat_Stack);

/* 为两个子正则表达式执行连接运算 */
void LinkAutomat(Stack *Automat_Stack);

/* 为两个子正则表达式执行或运算 */
void ParallelAutomat(Stack *Automat_Stack);

/*
 * 正则表达式语法匹配驱动程序, 通过语法分析自动机为相应的正则表达式构造正则转换自动机, 并返回自动机起始点
 * */
Vertex *RegexDriver(const char *regex);

void DestroyGrammerTable(void);

#endif /* grammer_h */
