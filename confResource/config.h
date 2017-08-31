#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>
#include <mysql.h>

#define OPENLOG  1      //0=不记录日志   1=记录日志 

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

//定义KCode配置文件存放的路径
#define CONFIG_PATH     "../confResource.conf"
#define CONFIG_NOTESYMBOL    '#'
//存放库函数与资源的关系文件
#define FUNCLIBRARY_PATH  "../funcLibrary.conf"

#define CONFIG_KEY_MAX_NUM       50     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     30    //配置项value的最大值

#define MAX_SUBSTR               512   //拆分后子字符串的最大长度

#define DIRPATH_MAX              256

#define MAX_PROGRAMNAME_NUM      50     //a program max name length

#define MAX_FUNCNAME_LENGTH      50     //a function name max length

#define LOGINFO_LENGTH           1024 

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
extern char *createFuncCallTable;
extern char *deleteFuncCallTable;

//函数递归的最大深度
extern int max_funcCallRecursive_NUM;

//analyse program name
extern char programName[MAX_PROGRAMNAME_NUM];

typedef struct configurationScore
{
    int CPU;
    int MEM;
    int IO;
    int NET;
} confScore;

typedef struct functionList
{
    char funcName[MAX_FUNCNAME_LENGTH];
    int line;
    struct functionList *next;
} funcList;

#endif