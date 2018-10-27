/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : config.c
* Descripe     : global variable
******************************************************/

#include "config.h"

char programName[MAX_PROGRAMNAME_NUM] = "";

char srcPath[MAX_PATH_LENGTH] = "";

pthread_t convertSRCPthreadID[MAX_CONVERT_SRC_PTHREAD_NUM];
int ConvertSRCPthreadRet[MAX_CONVERT_SRC_PTHREAD_NUM] = { -1 };
pthread_t convertXMLPthreadID[MAX_CONVERT_XML_PTHREAD_NUM];
int ConvertXMLPthreadRet[MAX_CONVERT_XML_PTHREAD_NUM] = { -1 };
pthread_t insertXMLPthreadID[MAX_INSERT_XML_PTHREAD_NUM];
int InsertXMLPthreadRet[MAX_INSERT_XML_PTHREAD_NUM] = { -1 };

int totalConvertFileNum = 0;
int curConvertFileNum = 0;
int currentConvertSrcPthreadID = 0;
int currentConvertXmlPthreadID = 0;
int currentInsertXmlPthreadID = 0;
convertPthread_arg convSrcPthreadArg[MAX_CONVERT_SRC_PTHREAD_NUM];
convertPthread_arg convXmlPthreadArg[MAX_CONVERT_XML_PTHREAD_NUM];
char insXmlPthreadArg[MAX_INSERT_XML_PTHREAD_NUM][DIRPATH_MAX];