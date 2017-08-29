#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

//定义KCode配置文件存放的路径
#define CONFIG_PATH     "confResource.conf"
#define CONFIG_NOTESYMBOL    '#'

#define CONFIG_KEY_MAX_NUM       50     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     30    //配置项value的最大值

#define MAX_SUBSTR               512   //拆分后子字符串的最大长度

#endif