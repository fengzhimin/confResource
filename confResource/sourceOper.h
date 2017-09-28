/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : sourceOper.h
* Descripe     : parse source code
******************************************************/

#ifndef __SOURCEOPER_H__
#define __SOURCEOPER_H__

#include "dirOper.h"
#include "logOper.h"
#include "config.h"
#include "strOper.h"
#include "CXmlOper.h"
#include "CPPXmlOper.h"
#include "mysqlOper.h"
#include "buildTempTable.h"
#include "buildFuncLibrary.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <time.h>

/***************************************
 * func: get software config info(srcPath, rebuild, config variable)
 * return: true = success   false = failure
****************************************/
bool getSoftWareConfInfo();

/***************************************
 * func: get analyse program name
 * return: true = success   false = failure
 * @para sourcePath: source code directory
***************************************/
bool getProgramName(char *sourcePath);

/***************************************
 * func: judge whether C language preprocess file or not
 * return: true = yes   false = no
 * @para filePath: file Path
****************************************/
bool judgeCPreprocessFile(char *filePath);

/***************************************
 * func: judge whether C language source xml file or not
 * return: true = yes   false = no
 * @para filePath: file Path
****************************************/
bool judgeCSrcXmlFile(char *filePath);

/***************************************
 * func: judge whether C language header xml file or not
 * return: true = yes   false = no
 * @para filePath: file Path
****************************************/
bool judgeCHeaderXmlFile(char *filePath);

/***************************************
 * func: judge whether C Plus Plus language preprocess file or not
 * return: true = yes   false = no
 * @para filePath: file Path
****************************************/
bool judgeCPPPreprocessFile(char *filePath);

/***************************************
 * func: judge whether C Plus Plus language source xml file or not
 * return: true = yes   false = no
 * @para filePath: file Path
****************************************/
bool judgeCPPSrcXmlFile(char *filePath);

/***************************************
 * func: judge whether C Plus Plus language header xml file or not
 * return: true = yes   false = no
 * @para filePath: file Path
****************************************/
bool judgeCPPHeaderXmlFile(char *filePath);

/***************************************
 * func: convert source code to xml style
 * @return: true = success     false = failure
 * @para srcPath: source code path
 * @para desPath: save xml path
***************************************/
bool CodeToXML(char *srcPath, char *desPath);

/*****************************************
 * func: convert a program all source file to xml(input program directory) and extract self-define function
 * return: true = success   false = failure
 * @para dirPath: program directory
*****************************************/
bool convertProgram(char *dirPath);

/*****************************************
 * func: extract self-define function
 * return: true = success   false = failure
 * @para dirPath: program directory
*****************************************/
bool analyzeProgram(char *dirPath);

/*****************************************
 * func: get the total number of analyze file
 * return: total number
 * @para dirPath: program directory
*****************************************/
int getTotalAnalyzeFileNum(char *dirPath);

/********************************************
 * func: initial software source code
 * return: true = success   false = failure
 * @para srcPath: source code path
*********************************************/
bool initSoftware(char *srcPath);

/*********************************************
 * func: optimize database operation
 * return: void
*********************************************/
void optDataBaseOper(char *tempFuncScoreTableName, char *tempFuncCallTableName);

/*****************************************
 * func: delete temp xml file
 * return: true = success     false = failure
*****************************************/
bool deleteTempXMLFile();

/****************************************
 * func: build self-define function score
 * return: true = success    false = failure
****************************************/
bool buildFuncScore();

/****************************************
 * func: get variable used function
 * return: void
 * @para varName: variable name
 * @para xmlPath: xml file path
*****************************************/
void getVarUsedFunc(char *varName, char *xmlPath);

/****************************************
 * func: build configuration score
 * return: score
 * @para confName: configuration name
 * @para xmlPath: xml file path
*****************************************/
confScore buildConfScore(char *confName, char *xmlPath);

/****************************************
 * func: 配置项在一个函数中的资源分数(递归包含该函数所调用的函数)
 * return: score
 * @para funcName: function name
 * @para funcType: function type   true = static    false = extern
 * @para srcFile: function defined source file path
*****************************************/
confScore getFuncScore(char *funcName, bool funcType, char *argumentType, char *srcFile);

/****************************************
 * func: get configuration key influence variable and function info
 * return: void
 * @para confKeyName: configuration key name
 * @para dirPath: program path
****************************************/
void getConfKeyInfluence(char *confKeyName, char *dirPath);

#endif