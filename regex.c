//
//  regex.c
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/13.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#include "regex.h"

/* 为一个正则表达式构建一个正则表达式对象已进行正则检测 */
Vertex *CreateRegexObj(const char *regex)
{
    return RegexDriver(regex);
}

bool RegexMatch(const char *text, Vertex *automat)
{
    return AutomatDriver(text, automat);
}
