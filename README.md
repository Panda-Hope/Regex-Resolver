# Regex-Resolver
基于NFA(不确定有穷自动机)与自底向上语法分析构造的正则表达式解析器

### Regex-Resolver流程构造剖析:

### 基本原理
 首先将正则表达式理解为由某些字符串所组成的一种语言的集合, 进而对字符串的组成进行分析,  
 将其分割成基本的字符串、连接、或、乘积这几种基本的运算组合。  
 从而得到对于正则表达式本质的基本理解(即由某些特定的字符串及其通过连接、或、幂运算所构成的语言集的集合)。

### 正则表达式的NFA(不确定有穷自动机)
在继上面所得到的对于正则表达式的基本理解之后, 我们发现对于正则表达式的验证流程即从左至右依次验证的流程。  
于是我们便可将其视作为一种状态转换的流程，由此我们便可将上面所述的基本运算将其转换为NFA自动机的构建。

对于每一个基本符号(a)构建如下图的自动机:
<div align="space-between">
  <img src="https://github.com/Panda-Hope/panda-hope.github.io/blob/master/gif/regex1.png" width="293" height="109">
</div>
然后对于字符的连接、或、幂运算分别构造如下图所示的自动机:
<div align="space-between">
  <img src="https://github.com/Panda-Hope/panda-hope.github.io/blob/master/gif/regex4.png" width="617" height="157">
  <img src="https://github.com/Panda-Hope/panda-hope.github.io/blob/master/gif/regex2.png" width="658" height="253">
  <img src="https://github.com/Panda-Hope/panda-hope.github.io/blob/master/gif/regex3.png" width="645" height="283">
</div>
由此得到了基本的NFA自动机的模型。

### 自底向上的语法分析
首先根据上面的NFA自动机，我们得到了正则表达式的自动机模型，在验证之后确认此自动机可以被用来对于正则表达式的验证。  
由此我们剩下的便是对于正则表达式的语法进行解析，何时根据正则表达式的内容构建上面所述的NFA自动机模型。  
首先对于此正则表达式我们定义其可支持的数据类型以及操作如下:

 ##### 数据类型:
 * . : 匹配任意一个非换行符符号
 * \d: 匹配0~9的数字
 * \t: 匹配制表符
 * \s: 匹配一个空白字符，包括空格、制表符、换页符和换行符
 * \w: 匹配一个单字字符（字母、数字或者下划线）等价于[A-Za-z0-9_]
 ##### 运算类型:
 * (a): 创建一个子正则表达式a
 * a* : 匹配多次或零次
 * a|b: 匹配a或者b
 
 由此我们便可设计一个增强文法Q如下来代表此正则表达式规则的语法:
 ##### 正则表达式文法:
   * Q -> S           对文法S的增强文法 
   * S -> S or T | T (用字母or替代|避免与文法分隔符"|"冲突) 
   * T -> TE | E 
   * E -> E* | F
   * F -> id | (S)


