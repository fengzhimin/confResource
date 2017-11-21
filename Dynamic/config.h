/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : config.h
* Descripe     : global variable
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>
#include <pthread.h>

#define OPENLOG  1      //0=不记录日志   1=记录日志

#define DEBUG    0      //0=不打印调试信息    1 = 打印调试信息

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

#define MAX_SUBSTR               512   //拆分后子字符串的最大长度

#define DIRPATH_MAX              256

#define LOGINFO_LENGTH           1024 


#endif