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
char *createFuncLibraryTable = "create table funcLibrary(funcName varchar(128), type varchar(128), score int)";
//存放已经打分的函数
//每个函数对应的各项资源的分数
//type: extern 全局的函数    static 局部的函数
char *createFuncScoreTable = "create table funcScore(funcName varchar(128), type varchar(128) default 'extern', sourceFile text, line int,\
        CPU int default 0, MEM int  default 0, IO int default 0, NET int default 0)";
char *deleteFuncScoreTable = "drop table if exists funcScore";
//存放函数调用关系图
//funcName: 自定义函数
//sourceFile: 自定义函数的源文件
//calledFunc: funcName调用的函数
//calledFuncType: 被调用函数的类型
//line: 调用的行号
//type: calledFunc类型  'L' = 库函数    'S' = 自定义函数
char *createFuncCallTable = "create table funcCall(funcName varchar(128), funcCallType varchar(128) default 'extern', \
        sourceFile text, calledFunc varchar(128), calledFuncType varchar(128) default 'extern', CalledSrcFile text, line int, type char(1))";
char *deleteFuncCallTable = "drop table if exists funcCall";

char programName[MAX_PROGRAMNAME_NUM] = "";

int max_funcCallRecursive_NUM = 5;