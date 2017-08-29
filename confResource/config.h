#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>
#include <mysql.h>

#define OPENLOG  1      //0=不记录日志   1=记录日志 

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

//定义KCode配置文件存放的路径
#define CONFIG_PATH     "confResource.conf"
#define CONFIG_NOTESYMBOL    '#'

#define CONFIG_KEY_MAX_NUM       50     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     30    //配置项value的最大值

#define MAX_SUBSTR               512   //拆分后子字符串的最大长度

extern char bind_address[CONFIG_VALUE_MAX_NUM];
extern int port;
extern char user[CONFIG_VALUE_MAX_NUM];
extern char pass[CONFIG_VALUE_MAX_NUM];
extern char database[CONFIG_VALUE_MAX_NUM];
extern MYSQL db;
extern MYSQL *mysqlConnect;
extern char *createFuncLibraryTable;
extern char *createFuncScoreTable;
extern char *deleteFuncScoreTable;

#endif