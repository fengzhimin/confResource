/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : config.c
* Descripe     : global variable
******************************************************/

#include "config.h"

//mysql info
char bind_address[CONFIG_VALUE_MAX_NUM] = "localhost";
int port = 3306;
char user[CONFIG_VALUE_MAX_NUM] = "root";
char pass[CONFIG_VALUE_MAX_NUM] = "fzm";
char database[CONFIG_VALUE_MAX_NUM] = "mysql";
MYSQL db;
MYSQL *mysqlConnect = NULL;
//建立函数与资源关系的库
//字段解释 funcName = 函数名称   type = 类型 分别为CPU、MEM、IO、NET
char *createFuncLibraryTable = "create table funcLibrary(funcName varchar(128) primary key, type varchar(128), score int)";
char *deleteFuncLibraryTable = "drop table if exists funcLibrary";
//存放已经打分的函数
//每个函数对应的各项资源的分数
//type: extern 全局的函数    static 局部的函数
char *createFuncScoreTableTemplate = "create table %s(funcName varchar(128), type varchar(128) default 'extern', argumentType varchar(128), \
        sourceFile varchar(128), line int, CPU int default 0, MEM int  default 0, IO int default 0, NET int default 0, index(funcName, type, \
        argumentType, sourceFile))";
char *deleteFuncScoreTableTemplate = "drop table if exists %s";
char *createTempFuncScoreTableTemplate = "create table %s(funcName varchar(128), type varchar(128) default 'extern', argumentType text, \
        sourceFile text, line int, CPU int default 0, MEM int  default 0, IO int default 0, NET int default 0, index(funcName, type))";
char *deleteTempFuncScoreTableTemplate = "drop table if exists %s";
//存放类继承的关系
//每个函数对应的各项资源的分数
//type: extern 全局的函数    static 局部的函数
char *createClassInheritTableTemplate = "create table %s(className varchar(128) primary key, inheritType varchar(128) default 'public', \
        inheritClassName varchar(128))";
char *deleteClassInheritTableTemplate = "drop table if exists %s";
//存放函数调用关系图
//funcName: 自定义函数
//sourceFile: 自定义函数的源文件
//calledFunc: funcName调用的函数
//calledFuncType: 被调用函数的类型
//line: 调用的行号
//type: calledFunc类型  'L' = 库函数    'S' = 自定义函数
//forNum: nested for number
//whileNum: nested while or do-while number
char *createFuncCallTableTemplate = "create table %s(funcName varchar(128), funcCallType varchar(128) default 'extern', argumentType varchar(128), \
        sourceFile varchar(128), calledFunc varchar(128), calledFuncType varchar(128) default 'extern', calledFuncArgumentType text, \
        CalledSrcFile text, line int, type char(1), forNum int default 0, whileNum int default 0, index(funcName, funcCallType, argumentType, \
        sourceFile, calledFunc, line))";
char *deleteFuncCallTableTemplate = "drop table if exists %s";
char *createTempFuncCallTableTemplate = "create table %s(funcName varchar(128), funcCallType varchar(128) default 'extern', argumentType text, \
        sourceFile text, calledFunc varchar(128), calledFuncType varchar(128) default 'extern', calledFuncArgumentType text, \
        CalledSrcFile text, line int, type char(1), forNum int default 0, whileNum int default 0, index(funcName, funcCallType, calledFunc))";
char *deleteTempFuncCallTableTemplate = "drop table if exists %s";

char funcScoreTableName[MAX_PROGRAMNAME_NUM*2] = "";
char tempFuncScoreTableName[MAX_PTHREAD_NUM][MAX_PROGRAMNAME_NUM*2] = {"", "", "", ""};
char classInheritTableName[MAX_PROGRAMNAME_NUM*2] = "";
char funcCallTableName[MAX_PROGRAMNAME_NUM*2] = "";
char tempFuncCallTableName[MAX_PTHREAD_NUM][MAX_PROGRAMNAME_NUM*2] = {"", "", "", ""};

char programName[MAX_PROGRAMNAME_NUM] = "";

int max_funcCallRecursive_NUM = 5;

char currentAnalyseXmlPath[DIRPATH_MAX] = "";

char srcPath[MAX_PATH_LENGTH] = "";
bool rebuild = true;
confOptMap *beginConfOpt = NULL;
confOptMap *endConfOpt = NULL;
confOptMap *currentConfOpt = NULL;

pthread_t pthreadID[MAX_PTHREAD_NUM];
int pthreadRet[MAX_PTHREAD_NUM] = { -1 };
pthread_arg pthreadArg[MAX_PTHREAD_NUM];