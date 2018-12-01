//
//  grammer.c
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/16.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#include "grammer.h"

ItemsArc *InitItemsArc(void)
{
    ItemsArc *arc = malloc(sizeof(ItemsArc));
    arc->c = 0;
    arc->next = NULL;
    arc->items = NULL;

    return arc;
}

/* 初始化语法分析自动机状态数据 */
SetOfItems *InitSetOfItems(void)
{
    SetOfItems *items = malloc(sizeof(SetOfItems));
    items->SetOfItem_Stack = InitStack();
    items->arc = NULL;
    
    return items;
}

//void FreeSetOfItems(SetOfItems *items)
//{
//    if (!items) return;
//    
//    FreeStack(items->SetOfItem_Stack);
//    
//    ItemsArc *arc = items->arc;
//    while (arc) {
//        FreeSetOfItems(arc->items);
//        
//        ItemsArc *t = arc;
//        arc = arc->next;
//        free(t);
//        t = NULL;
//    }
//    
//    free(items);
//    items = NULL;
//}

/* 为每一个形如: A->α 的文法产生式构造一个DFA自动机 */
void CreateGrammerAutomat(Grammer *g)
{
    Automat *s = CreateArc(*g->body);
    int i = 1;
    
    while(*(g->body+i) != '\0') {
        Automat *t = CreateArc(*(g->body+i));
        s = Link(s, t);
        i++;
    }

    g->automat = s;
}

void InitGrammerAutomat(void)
{
    /* 为每一个文法产生式创建一个自动机 */
    for (int i = 0;i<grammerSize;i++) {
        CreateGrammerAutomat(&grammers[i]);
    }
}

/*
 * 对某个语法状态求其闭包即对其中的“项”求与之其相等的"项"的集合
 * items: 语法自动机中的一个闭包集
 */
void GetClosureSetOfItems(SetOfItems *items)
{
    Stack *Vertex_Stack = items->SetOfItem_Stack;
    size_t vertexSize = Vertex_Stack->top - Vertex_Stack->base;
    
    Stack *GrammerTag_Stack = InitStack();
    
    for (size_t i=1;i<=vertexSize;i++) {
        Vertex *v = (Vertex *)*(Vertex_Stack->top-i);
        if (v->arc) Push(GrammerTag_Stack, &v->arc->c);
    }
    
    int count = 0;                                                                   // 将已经查询过的文法消去，下降匹配
    while (!EmptyStack(GrammerTag_Stack)) {
        char tag = *(char *)Pop(GrammerTag_Stack);
        
        for (int i = count;i<grammerSize;i++) {
            if (grammers[i].header == tag) {
                count = i + 1;
                
                Push(GrammerTag_Stack, &grammers[i].automat->f->arc->c);             // 堆栈继续查找相同的项
                Push(items->SetOfItem_Stack, grammers[i].automat->f);                // 将与当前栈顶项相同的项推入其中
            }
        }
    }
    
    FreeStack(GrammerTag_Stack);
}

/* 返回某个闭包状态在输入文法符号C之后得到后继闭包集 */
SetOfItems *Goto(SetOfItems *current, char c)
{
    SetOfItems *items = InitSetOfItems();
    Stack *Vertex_Stack = current->SetOfItem_Stack;
    size_t vertexSize = Vertex_Stack->top - Vertex_Stack->base;
    
    /* 将当前项集中匹配正则符号成功的点移入新的状态集中 */
    for (size_t i=1;i<=vertexSize;i++) {
        Vertex *v = (Vertex *)*(Vertex_Stack->top-i);
        
        if (v->arc && (v->arc->c == c)) {
            Push(items->SetOfItem_Stack, v->arc->v);
        }
    }
    
    GetClosureSetOfItems(items);

    return items;
}

/* 检测两个状态闭包集是否相等 */
bool CheckItemsEqual(SetOfItems *a, SetOfItems *b)
{
    size_t sizeA = a->SetOfItem_Stack->top - a->SetOfItem_Stack->base;
    size_t sizeB = b->SetOfItem_Stack->top - b->SetOfItem_Stack->base;
    
    if (sizeA != sizeB) return false;
    
    for (size_t i=1;i<=sizeA;i++) {
        Vertex *vertexA = (Vertex *)*(a->SetOfItem_Stack->top - i);
        Vertex *vertexB = (Vertex *)*(b->SetOfItem_Stack->top - i);
        
        if (vertexA != vertexB) return false;
    }
    
    return true;
}

/*
 * 为上面所描述的文法构建一个正则表达式语法分析自动机
 * */
Stack *GetGrammerTable(void)
{
    InitGrammerAutomat();                                       // 初始化文法自动机
    
    /* 初始化第一个文法为: Q -> S 的闭包项集 */
    SetOfItems *startState = InitSetOfItems();
    Push(startState->SetOfItem_Stack, grammers[0].automat->f);
    GetClosureSetOfItems(startState);
    
    /* 为文法S构建构建闭包项目集群族 */
    Stack *SetOfItems_Stack = InitStack();
    Push(SetOfItems_Stack, startState);

    for (size_t i=0;i<SetOfItems_Stack->top-SetOfItems_Stack->base;i++) {
        SetOfItems *currentItems = (SetOfItems *)*(SetOfItems_Stack->base + i);
        ItemsArc *currentItemsArc = currentItems->arc;
        
        for (int j=0;j<sizeof(grammerCharacters)/sizeof(grammerCharacters[0]);j++) {
            SetOfItems *nextItems = Goto(currentItems, grammerCharacters[j]);
            
            /* 如果后继项目集为空直接跳过 */
            if (nextItems->SetOfItem_Stack->top - nextItems->SetOfItem_Stack->base == 0) continue;
            
            /* 仅当不存在相等闭包集时将其推入栈中 */
            bool hasEqualItems = false;
            for (size_t k=0;k<SetOfItems_Stack->top-SetOfItems_Stack->base;k++) {
                SetOfItems *cmpItems = (SetOfItems *)*(SetOfItems_Stack->base + k);

                if (CheckItemsEqual(nextItems, cmpItems)) {
                    hasEqualItems = true;
                    nextItems = cmpItems;
                    break;
                };
            }

            if (!hasEqualItems) Push(SetOfItems_Stack, nextItems);
            
            /* 将当前状态项集通过连接弧连接下一个对应文法符号可转换状态项集 */
            if (!currentItemsArc) {
                currentItemsArc = currentItems->arc = InitItemsArc();
            }else {
                currentItemsArc = currentItemsArc->next = InitItemsArc();
            }
            currentItemsArc->c = grammerCharacters[j];
            currentItemsArc->items = nextItems;
        }
    }
    
    /* 闭包项集状态检验程序 */
//    SetOfItems *test = (SetOfItems *)*(SetOfItems_Stack->base+10);
//    test = test->arc->items;
//    size_t size = test->SetOfItem_Stack->top - test->SetOfItem_Stack->base;

    // 测验闭包项集中数据是否正确
//    for (int i=0;i<size;i++) {
//        Vertex *v = (Vertex *)*(test->SetOfItem_Stack->base + i);
//        if (v->arc) printf("%c ", v->arc->c);
//    }
//    printf("\n");
    
    // 测验闭包项连接弧是否正确
//    ItemsArc *itemsArc = test->arc;
//    while (itemsArc) {
//        printf("%d ", itemsArc->c);
//        itemsArc = itemsArc->next;
//    }
//    printf("\n");

    return SetOfItems_Stack;
}

/* 词法分析程序，将正则表达式转变为词法单元队列 */
Queue *LexeAnalysis(const char *regex)
{
    Queue *LexeItem_Queue = InitQueue();
    
    while (*regex != '\0') {
        LexeItem *lexeItem =malloc(sizeof(LexeItem));
        lexeItem->fn = NULL;
        
        if (!lexeItem) {
            printf("词法单元内存分配失败\n");
            exit(1);
        }
        
        /* 当语法符号为(、)、|、* 时返回相应的词法单元类型，其余符号均为数据类型返回i及其实际数据值或基本数据类型函数 */
        switch (*regex) {
            case '|':
            case '*':
            case '(':
            case ')':
                lexeItem->type = *regex;
                break;
            case '.':
                lexeItem->type = 'i';
                lexeItem->fn = NonLineBreak;
                break;
            case '\\':
                lexeItem->type = 'i';
                
                /* 对基本数据类型进行匹配 */
                switch (*++regex) {
                    case 'd':
                        lexeItem->fn = IsDigit;
                        break;
                    case 't':
                        lexeItem->fn = MatchTab;
                        break;
                    case 's':
                        lexeItem->fn = MatchWhiteSpace;
                        break;
                    case 'w':
                        lexeItem->fn = MatchSingle;
                    default:
                        lexeItem->data = *regex;
                        break;
                }
                break;
            default:
                lexeItem->type = 'i';
                lexeItem->data = *regex;
                break;
        }
        
        EnQueue(LexeItem_Queue, lexeItem);
        regex++;
    }
    
    return LexeItem_Queue;
}

/* 为一个基本的正则表达式数据类型符号，构建一个基本的正则表达式自动机模型 */
void BasicAutomat(Stack *Automat_Stack, LexeItem *lexeItem)
{
    Automat *automat = lexeItem->fn ? CreateDataArc(lexeItem->fn) : CreateArc(lexeItem->data);
    Push(Automat_Stack, automat);
}

/* 为一个子正则表达式执行幂运算 */
void MutipleAutomat(Stack *Automat_Stack)
{
    Automat *a = Pop(Automat_Stack);
    Automat *mutiple = Multiple(a);
    Push(Automat_Stack, mutiple);
}

/* 为两个子正则表达式执行连接运算 */
void LinkAutomat(Stack *Automat_Stack)
{
    Automat *a = Pop(Automat_Stack);
    Automat *b = Pop(Automat_Stack);
    Automat *linked = Link(b, a);
    Push(Automat_Stack, linked);
}

/* 为两个子正则表达式执行或运算 */
void ParallelAutomat(Stack *Automat_Stack)
{
    Automat *a = Pop(Automat_Stack);
    Automat *b = Pop(Automat_Stack);
    Automat *paralled = Or(b, a);
    Push(Automat_Stack, paralled);
}

/*
 * 正则表达式语法匹配驱动程序, 通过语法分析自动机为相应的正则表达式构造正则转换自动机, 并返回自动机起始点
 * */
Vertex *RegexDriver(const char *regex)
{
    /* 初始化驱动程序相关数据结构 */
    if (!GrammerTable_Stack) GrammerTable_Stack = GetGrammerTable();        // 初始化正则表达式文法S语法分析自动机
    
    Queue *LexeItem_Queue = LexeAnalysis(regex);                            // 将正则表达式符号串转变为词法单元队列
    Stack *Automat_Stack = InitStack();                                     // 正则表达式自动机栈，代表当前语法分析程序所构造的正则表达式自动机
    Stack *SetOfItems_Stack = InitStack();                                  // 语法自动机状态栈，代表当前所处的状态
    LexeItem *currentLexeItem = NULL;                                       // 当前语法分析流所处的正则表达式词法单元项
    
    SetOfItems *start = (SetOfItems *)*(GrammerTable_Stack->base);
    Push(SetOfItems_Stack, start);
    
    /* 开始执行语法匹配 */
    while (true) {
        SetOfItems *current = GetTop(SetOfItems_Stack);
        ItemsArc *itemsArc = current->arc;
        bool moveIn = false;                                                // 检测动作是移入还是归约
        
        /* 执行移入动作 */
        while (itemsArc && !EmptyQueue(LexeItem_Queue)) {
            LexeItem *lexeItem = GetHead(LexeItem_Queue);
            
            if (itemsArc->c == lexeItem->type) {
                Push(SetOfItems_Stack, itemsArc->items);
                currentLexeItem = DeQueue(LexeItem_Queue);
                moveIn = true;
                break;
            }
            itemsArc = itemsArc->next;
        }
        
        /* 执行归约、完成、报错 */
        if (!moveIn) {
            size_t stateSize = current->SetOfItem_Stack->top - current->SetOfItem_Stack->base;
            bool reduced = false;
            
            for (size_t i=0;i<stateSize;i++) {
                Vertex *v = (Vertex *)*(current->SetOfItem_Stack->base + i);
                
                /*
                 * 当顶点后续弧为空时，判断达到某个文法产生式的最右端，执行归约动作
                 * 此时文法的设计当不存在归约与归约的冲突, 否则将产生错误
                 */
                if (!v->arc) {
                    reduced = true;
                    
                    /* 查找需要执行归约动作的文法 */
                    Grammer *reducedGrammer = NULL;
                    for (int j=0;j<grammerSize;j++) {
                        if (grammers[j].automat->r == v) {
                            reducedGrammer = &grammers[j];
                            break;
                        }
                    }
                    
                    /* 开始执行归约动作 */
                    for (int k=0;k<reducedGrammer->size;k++) {
                        Pop(SetOfItems_Stack);
                    }
                    
                    SetOfItems *current = GetTop(SetOfItems_Stack);
                    ItemsArc *itemsArc = current->arc;
                    
                    while (itemsArc) {
                        if (itemsArc->c == reducedGrammer->header) {
                            Push(SetOfItems_Stack, itemsArc->items);
                            break;
                        }
                        itemsArc = itemsArc->next;
                    }
                    
                    /* 正则表达式自动机模式匹配，为相应的正则表达式构建相应的自动机 */
                    switch (reducedGrammer->count) {
                        case 7:
                            BasicAutomat(Automat_Stack, currentLexeItem);
                            break;
                        case 5:
                            MutipleAutomat(Automat_Stack);
                            break;
                        case 3:
                            LinkAutomat(Automat_Stack);
                            break;
                        case 1:
                            ParallelAutomat(Automat_Stack);
                            break;
                    }
                    
                    /* 文法匹配测试程序 */
//                    printf("%s -> %c\n", reducedGrammer->body, reducedGrammer->header);
                    
                    /* 匹配到文法开始符号Q，正则语法匹配程序成功，结束程序执行 */
                    if (reducedGrammer->header == grammers[0].header) {
                        Automat *final = Pop(Automat_Stack);
                        
                        FreeQueue(LexeItem_Queue);
                        FreeStack(Automat_Stack);
                        FreeStack(SetOfItems_Stack);
                        
                        return final->f;
                    }
                }
            }
            
            /* 归约失败，检测到语法错误 */
            if (!reduced) {
                printf("正则表达式语法错误\n");
                exit(1);
            }
        }
    }
}

void DestroyGrammerTable(void)
{
    
}
