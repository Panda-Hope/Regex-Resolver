//
//  regex.h
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/13.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#ifndef regex_h
#define regex_h

#include <stdbool.h>
#include "automat.h"
#include "grammer.h"

/* 为一个正则表达式构建一个正则表达式对象已进行正则检测 */
Vertex *CreateRegexObj(const char *regex);

bool RegexMatch(const char *text, Vertex *automat);

#endif /* regex_h */
