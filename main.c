//
//  main.c
//  RegexCompiler
//
//  Created by 臻华的Macbook pro on 2018/11/12.
//  Copyright © 2018 臻华的Macbook pro. All rights reserved.
//

#include <stdio.h>
#include "regex.h"

/*
 * 正则表达式程序使用示例
 * */
int main(int argc, const char * argv[]) {
    /* 手机号码校验 */
//    Vertex *phoneRegexObj = CreateRegexObj("1(3|4|5|7|8)\\d\\d\\d\\d\\d\\d\\d\\d\\d");
//    bool result1 = RegexMatch("15973850842", phoneRegexObj);
//    bool result2 = RegexMatch("13333", phoneRegexObj);
    
//    printf("%s \n", result1 ? "true" : "false");
//    printf("%s \n", result2 ? "true" : "false");
    
    /* 普通校验 */
//    Vertex *regexObj = CreateRegexObj("a(b|c)*666t*");
//    bool result3 = RegexMatch("accbb666", regexObj);
//    bool result4 = RegexMatch("a666tttttt", regexObj);
    
//    printf("%s \n", result3 ? "true" : "false");
//    printf("%s \n", result4 ? "true" : "false");
    
    Vertex *regex = CreateRegexObj(argv[0]);
    bool result = RegexMatch(argv[1], regex);
    
    printf("%s", result ? "true" : "false");
    
    return 0;
}
