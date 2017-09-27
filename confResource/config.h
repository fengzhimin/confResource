/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : config.h
* Descripe     : global variable
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>
#include <mysql.h>

#define OPENLOG  1      //0=不记录日志   1=记录日志

#define DEBUG    0      //0=不打印调试信息    1 = 打印调试信息

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

//定义KCode配置文件存放的路径
#define CONFIG_PATH     "../confResource.conf"
#define CONFIG_NOTESYMBOL    '#'
//存放库函数与资源的关系文件
#define FUNCLIBRARY_PATH  "../funcLibrary.conf"

#define INPUT_PATH        "../input.conf"

#define CONFIG_KEY_MAX_NUM       50     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     30    //配置项value的最大值

#define MAX_SUBSTR               512   //拆分后子字符串的最大长度

#define DIRPATH_MAX              256

#define MAX_PROGRAMNAME_NUM      128     //a program max name length

#define MAX_FUNCNAME_LENGTH      128     //a function name max length

#define MAX_VARIABLE_LENGTH      128     //a variable name max length

#define MAX_PATH_LENGTH          1024    //path max length

#define FORNUM                   0     //for multiple
#define WHILENUM                 0     //while and do-while multiple

#define LOGINFO_LENGTH           1024 

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
    bool funcType;     //false = extern    true = static
    char argumentType[512];
    int line;
    char sourceFile[DIRPATH_MAX];
    struct functionList *next;
} funcList;

typedef struct variableDef
{
    char varName[MAX_VARIABLE_LENGTH];
    bool type;  //false = local variable   true = global variable
    int line;
    struct variableDef *next;
} varDef;

typedef struct functionCallList
{
    char funcName[MAX_FUNCNAME_LENGTH];
    char argumentType[512];
    int line;
    char sourceFile[DIRPATH_MAX];
    struct functionCallList *next;
} funcCallList;

typedef struct variableType
{
    char varName[MAX_VARIABLE_LENGTH];
    char type[MAX_VARIABLE_LENGTH];
    int line;
    struct variableType *next;
} varType;

/*********************************
 * func: config option map relationship
 * @para confName: config option name
 * @para confVarName: config option map variable name
 * @para mapVariableNum: the number of maped config variable
*********************************/
typedef struct configOptionMap
{
    char confName[MAX_VARIABLE_LENGTH];
    char (*confVarName)[MAX_SUBSTR];
    int mapVariableNum;
    struct configOptionMap *next;
} confOptMap;

extern char bind_address[CONFIG_VALUE_MAX_NUM];
extern int port;
extern char user[CONFIG_VALUE_MAX_NUM];
extern char pass[CONFIG_VALUE_MAX_NUM];
extern char database[CONFIG_VALUE_MAX_NUM];
extern MYSQL db;
extern MYSQL *mysqlConnect;
extern char *createFuncLibraryTable;
extern char *deleteFuncLibraryTable;
extern char *createFuncScoreTableTemplate;
extern char *deleteFuncScoreTableTemplate;
extern char *createTempFuncScoreTableTemplate;
extern char *deleteTempFuncScoreTableTemplate;
extern char *createClassInheritTableTemplate;
extern char *deleteClassInheritTableTemplate;
extern char *createFuncCallTableTemplate;
extern char *deleteFuncCallTableTemplate;
extern char *createTempFuncCallTableTemplate;
extern char *deleteTempFuncCallTableTemplate;

extern char funcScoreTableName[MAX_PROGRAMNAME_NUM*2];
extern char tempFuncScoreTableName[MAX_PROGRAMNAME_NUM*2];
extern char classInheritTableName[MAX_PROGRAMNAME_NUM*2];
extern char funcCallTableName[MAX_PROGRAMNAME_NUM*2];
extern char tempFuncCallTableName[MAX_PROGRAMNAME_NUM*2];

extern char srcPath[MAX_PATH_LENGTH];
extern bool rebuild;
extern confOptMap *beginConfOpt;
extern confOptMap *endConfOpt;
extern confOptMap *currentConfOpt;

//函数递归的最大深度
extern int max_funcCallRecursive_NUM;

//analyse program name
extern char programName[MAX_PROGRAMNAME_NUM];

//current analyse xml file path
extern char currentAnalyseXmlPath[DIRPATH_MAX];

#endif