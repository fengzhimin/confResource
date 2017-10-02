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
#include <pthread.h>

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

//convert source to xml file pthread number
#define MAX_CONVERT_SRC_PTHREAD_NUM  10
//analyze xml file pthread number
#define MAX_ANALYZE_XML_PTHREAD_NUM  20
//analyze config option pthread number
#define MAX_ANALYZE_CONFOPT_PTHREAD_NUM  1

typedef struct configurationScore
{
    int CPU;
    int MEM;
    int IO;
    int NET;
} confScore;

typedef struct functionInfo
{
    char funcName[MAX_FUNCNAME_LENGTH];
    char funcType[7];     //false = extern    true = static
    char argumentType[512];
    char sourceFile[DIRPATH_MAX];
    struct functionInfo *next;
} funcInfo;

typedef struct variableDef
{
    char varName[MAX_VARIABLE_LENGTH];
    bool type;  //false = local variable   true = global variable
    int line;
    struct variableDef *next;
} varDef;

/****************************
 * func: function call relationship
 * @para funcName: function name
 * @para argumentType: funcName argument type
 * @para calledLine: funcName called line
 * @para sourceFile: funcName defined source file
****************************/
typedef struct functionCallList
{
    char funcName[MAX_FUNCNAME_LENGTH];
    char funcType[7];     //false = extern    true = static
    char argumentType[512];
    int calledLine;
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

typedef struct convertSrcPthread_argument
{
    char src_dir[DIRPATH_MAX];
    char des_dir[DIRPATH_MAX];
    int pthreadID;
} convertSrcPthread_arg;

typedef struct analyzeXmlPthread_argument
{
    char src_dir[DIRPATH_MAX];
    int pthreadID;
} analyXmlPthread_arg;

typedef struct analyzeConfOptPthread_argument
{
    char confOptName[MAX_VARIABLE_LENGTH];
    char xmlFilePath[DIRPATH_MAX];
    int pthreadID;
} analyConfOptPthread_arg;

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
extern char tempFuncScoreTableName[MAX_ANALYZE_XML_PTHREAD_NUM][MAX_PROGRAMNAME_NUM*2];
extern char classInheritTableName[MAX_PROGRAMNAME_NUM*2];
extern char funcCallTableName[MAX_PROGRAMNAME_NUM*2];
extern char tempFuncCallTableName[MAX_ANALYZE_XML_PTHREAD_NUM][MAX_PROGRAMNAME_NUM*2];

extern char srcPath[MAX_PATH_LENGTH];
extern bool rebuild;
extern confOptMap *beginConfOpt;
extern confOptMap *endConfOpt;
extern confOptMap *currentConfOpt;
extern int confOptNum;

//函数递归的最大深度
extern int max_funcCallRecursive_NUM;

//analyse program name
extern char programName[MAX_PROGRAMNAME_NUM];

//current analyse xml file path
extern char currentAnalyseXmlPath[DIRPATH_MAX];

extern pthread_t analyzeXMLPthreadID[MAX_ANALYZE_XML_PTHREAD_NUM];
extern int analyzeXMLPthreadRet[MAX_ANALYZE_XML_PTHREAD_NUM];
extern pthread_t convertSRCPthreadID[MAX_CONVERT_SRC_PTHREAD_NUM];
extern int ConvertSRCPthreadRet[MAX_CONVERT_SRC_PTHREAD_NUM];
extern pthread_t analyzeConfOptPthreadID[MAX_ANALYZE_CONFOPT_PTHREAD_NUM];
extern int analyzeConfOptPthreadRet[MAX_ANALYZE_CONFOPT_PTHREAD_NUM];

extern convertSrcPthread_arg convSrcPthreadArg[MAX_CONVERT_SRC_PTHREAD_NUM];
extern analyXmlPthread_arg analyXmlPthreadArg[MAX_ANALYZE_XML_PTHREAD_NUM];
extern analyConfOptPthread_arg analyConfOptPthreadArg[MAX_ANALYZE_CONFOPT_PTHREAD_NUM];
extern int funcCallCount[MAX_ANALYZE_CONFOPT_PTHREAD_NUM];
extern int totalConvertSrcFileNum;
extern int totalAnalyzeXmlFileNum;
extern int curConvertSrcFileNum;
extern int curAnalyzeXmlFileNum;
extern int currentAnalyzeXmlPthreadID;
extern int currentConvertSrcPthreadID;
extern int currentAnalyzeConfOptPthreadID;

#endif