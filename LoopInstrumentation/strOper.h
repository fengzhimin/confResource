/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : strOper.h
* Descripe     : common string operation
******************************************************/

#ifndef __STROPER_H__
#define __STROPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

/**********************************
 * func: 移除字符串中所有的指定字符
 * return: void
 * @para str: 处理的字符串数组
 * @para ch: 要被移除的字符
 * example: str = "1 2 3" ch = ' '  ------>  str = "123"
***********************************/
void removeChar(char *str, char ch);

/**********************************
 * func: 移除字符串中所有的数字字符
 * return: void
 * @para str: 处理的字符串数组
 * example: str = "1a 2b 3c"  ------>  str = "a b c"
***********************************/
void removeNum(char *str);

/**********************************
 * func: 去掉字符串中开头的所有的空格和制表符
 * return: void
 * @para str: 要被处理的字符串数组
 * example: str = "  str"    ---->    str = "str"
***********************************/
void removeBeginSpace(char *str);

/**********************************
 * func: get the number of specific char from string
 * return: the number of specific char
 * @para str: string
 * @para ch: finded char
***********************************/
int getSpecCharNumFromStr(char *str, char ch);

/**********************************
 * func: 通过字符ch来拆分字符串
 * return: 1 = 不可拆分   拆分后子字符串的个数 > 1
 * @para str: 要被拆分的字符串
 * @para ch: 分隔字符
 * @para subStr: 存放拆分后的子字符串的数组
 * @para subStrLength: subStr的大小
 * example: str = str1:str2:str3  --->  ch = ':'时 subStr = str1 str2 str3
***********************************/
int cutStrByLabel(char *str, char ch, char subStr[][MAX_SUBSTR], int subStrLength);

/**********************************
 * func: 将整数转化为字符串
 * return: 存放最后一个字符的下一个字符的地址
 * @para str: 存放转化后的字符串
 * @para num: 整数
 * example: num = 12 -------> str = "12"
***********************************/
char* IntToStr(char *str, int num);

/**********************************
 * func: 将字符串整数转化为int整数(正数)
 * return: 转化后的int
 * @para str: 存放转化前的字符串
 * example: str = "12" -------> return = 12
***********************************/
int StrToInt(char *str);

/***********************************
 * func: extract last char position from string
 * return: -1 = not exist char     >= 0 = last char position
 * @para str: string
 * @para ch: extract char
***********************************/
int ExtractLastCharIndex(char *str, char ch);

/**********************************
 * func: extract string from string literal
 * return: string point
 * @para str: string literal
 * example: ""maxmemory""     ---->   "maxmemory"
***********************************/
char *ExtractStringFromLiteral(char *str);

/**********************************
 * func: char[] replace char ch1 to char ch2
 * return: replace number
 * @para str: char []
 * @para ch1: old char
 * @para ch2: new char
**********************************/
int replaceChar(char *str, char ch1, char ch2);

#endif