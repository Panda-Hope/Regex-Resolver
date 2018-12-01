//
//  main.c
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/12.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#include <stdio.h>
#include "regex.h"

int main(int argc, const char * argv[]) {
    /* 正则表达式程序使用示例 */
    
    const char *regex1 = "5*";
    Vertex *regexObj1 = CreateRegexObj(regex1);
    bool result1 = RegexMatch("55555", regexObj1);

    printf("%s \n", result1 ? "true" : "false");
    
    const char *regex2 = "\\t";
    Vertex *regexObj2 = CreateRegexObj(regex2);
    bool result2 = RegexMatch("\t", regexObj2);
    bool result3 = RegexMatch("66", regexObj2);
    
    printf("%s \n", result2 ? "true" : "false");
    printf("%s \n", result3 ? "true" : "false");
    
    return 0;
}
