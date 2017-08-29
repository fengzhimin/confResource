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
char *createFuncLibraryTable = "create table funcLibray(funcName varchar(30), type text)";
//存放已经打分的函数
//每个函数对应的各项资源的分数
char *createFuncScoreTable = "create table funcScore(funcName varchar(30), CPU int, MEM int, IO int, NET int)";
char *deleteFuncScoreTable = "drop table funcScore";