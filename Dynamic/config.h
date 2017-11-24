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
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define OPENLOG  1      //0=不记录日志   1=记录日志

#define DEBUG    0      //0=不打印调试信息    1 = 打印调试信息

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

#define MAX_SUBSTR               512   //拆分后子字符串的最大长度

#define DIRPATH_MAX              256

#define LOGINFO_LENGTH           1024 

#define MAX_FUNCNAME_LENGTH      128

#define MAX_PATH_LENGTH          1024    //path max length

#define MAX_PROGRAMNAME_NUM      128     //a program max name length

//convert source to xml file pthread number
#define MAX_CONVERT_SRC_PTHREAD_NUM  10

//convert xml file to source file  pthread number
#define MAX_CONVERT_XML_PTHREAD_NUM  10

//insert xml file pthread number
#define MAX_INSERT_XML_PTHREAD_NUM   1

#define INPUT_PATH        "../input.conf"

/***************************************
 * @para blockNode: 函数体节点
 * @para funcName: 函数名
***************************************/ 
typedef struct SelfDefineFunctionNode
{
    xmlNodePtr blockNode;
    char funcName[MAX_FUNCNAME_LENGTH];
} SelfDefFuncNode;

typedef struct SelfDefineFunctionNodeList
{
    SelfDefFuncNode funcInfo;
    struct SelfDefineFunctionNodeList *next;
} SelfDefFuncNodeList;

typedef struct convertPthread_argument
{
    char src_dir[DIRPATH_MAX];
    char des_dir[DIRPATH_MAX];
    int pthreadID;
} convertPthread_arg;

extern char srcPath[MAX_PATH_LENGTH];

//analyse program name
extern char programName[MAX_PROGRAMNAME_NUM];

extern pthread_t convertSRCPthreadID[MAX_CONVERT_SRC_PTHREAD_NUM];
extern int ConvertSRCPthreadRet[MAX_CONVERT_SRC_PTHREAD_NUM];
extern pthread_t convertXMLPthreadID[MAX_CONVERT_XML_PTHREAD_NUM];
extern int ConvertXMLPthreadRet[MAX_CONVERT_XML_PTHREAD_NUM];
extern pthread_t insertXMLPthreadID[MAX_INSERT_XML_PTHREAD_NUM];
extern int InsertXMLPthreadRet[MAX_INSERT_XML_PTHREAD_NUM];
extern int totalConvertFileNum;
extern int curConvertFileNum;
extern int currentConvertSrcPthreadID;
extern int currentConvertXmlPthreadID;
extern int currentInsertXmlPthreadID;
extern convertPthread_arg convSrcPthreadArg[MAX_CONVERT_SRC_PTHREAD_NUM];
extern convertPthread_arg convXmlPthreadArg[MAX_CONVERT_XML_PTHREAD_NUM];
extern char insXmlPthreadArg[MAX_INSERT_XML_PTHREAD_NUM][DIRPATH_MAX];

#endif