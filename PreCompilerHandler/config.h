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
#include <errno.h>

#define OPENLOG  1      //0=不记录日志   1=记录日志

#define DEBUG    0      //0=不打印调试信息    1 = 打印调试信息

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

#define MAX_SUBSTR               512   //拆分后子字符串的最大长度

#define LOGINFO_LENGTH           1024 

#define DIRPATH_MAX              256

#define MAX_COMMAND_LENGTH       1024

#define MAX_FILENAME_LENGTH      64

//execute precompile command pthread number
#define MAX_EXECPRECOMPILE_PTHREAD_NUM  10

#define INPUT_PATH        "../input.conf"

/***************************************
 * @para dirPath: 当前预编译文件所在目录
 * @para funcName: 当前预编译指令
 * @para fileName: 当前预编译的文件名
***************************************/
typedef struct PreCompileInformation
{
    char dirPath[DIRPATH_MAX];
    char preCompileCommand[MAX_COMMAND_LENGTH];
    char fileName[MAX_FILENAME_LENGTH];
} PreCompileInfo;

typedef struct PreCompileInformationList
{
    PreCompileInfo info;
    struct PreCompileInformationList *next;
} PreCompileInfoList;

extern pthread_t exeCPreCompilePthreadID[MAX_EXECPRECOMPILE_PTHREAD_NUM];
extern int exeCPreCompilePthreadRet[MAX_EXECPRECOMPILE_PTHREAD_NUM];
extern int totalPreCompileFileNum;
extern int curPreCompileFileNum;
extern int currentPreCompilePthreadID;
extern char shellPath[MAX_EXECPRECOMPILE_PTHREAD_NUM][DIRPATH_MAX];
extern char JsonPath[DIRPATH_MAX];

#endif