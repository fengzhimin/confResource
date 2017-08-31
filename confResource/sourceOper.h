#ifndef __SOURCEOPER_H__
#define __SOURCEOPER_H__

#include "dirOper.h"
#include "logOper.h"
#include "config.h"
#include "strOper.h"
#include "xmlOper.h"
#include "mysqlOper.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <time.h>

/***************************************
 * func: get analyse program name
 * return: true = success   false = failure
 * @para sourcePath: source code directory
***************************************/
bool getProgramName(char *sourcePath);

/***************************************
 * func: judge whether C language source file or not
 * return: true = yes   false = no
 * @para filePath: file Path
****************************************/
bool judgeCSrcFile(char *filePath);

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
*****************************************/
confScore getFuncScore(char *funcName);

#endif