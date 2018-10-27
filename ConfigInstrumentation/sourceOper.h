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
#include "xmlOper.h"
#include "confOper.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <time.h>

/***************************************
 * func: get software config info(srcPath)
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
 * func: judge source file type(C/C++/java)
 * return: -1 = undefine file type   1 = C  2 = C++  3 = java
 * @para filePath: file Path
****************************************/
int judgeSrcFileType(char *filePath);

/***************************************
 * func: judge xml file type(C/C++/java)
 * return: -1 = undefine file type  1 = C  2 = C++  3 = java
 * @para filePath: file Path
****************************************/
int judgeXmlFileType(char *filePath);

/***************************************
 * func: convert source code to xml style or xml file to source code
 * @return: true = success     false = failure
 * @para srcPath: source code path/save xml path
 * @para desPath: save xml path/source code path
***************************************/
bool ExecSrcML(char *srcPath, char *desPath);

/***************************************
 * func: copy file from srcPath to desPath
 * @return: true = success     false = failure
 * @para srcPath: source file path
 * @para desPath: des file path
***************************************/
bool CpyFile(char *srcPath, char *desPath);

/*****************************************
 * func: get the total number of analyze file
 * return: total number
 * @para dirPath: program directory
*****************************************/
int getTotalConvertFileNum(char *dirPath);

/*****************************************
 * func: convert a program all source file to xml(input program directory)
 * return: true = success   false = failure
 * @para dirPath: program directory
*****************************************/
bool SrcToXML(char *dirPath);

/*****************************************
 * func: convert a program all xml to source file(input program directory)
 * return: true = success   false = failure
 * @para dirPath: program directory
*****************************************/
bool XMLToSrc(char *dirPath);

/*****************************************
 * func: 向xml文件插入记录循环次数的代码
 * return: true = success   false = failure
 * @para dirPath: xml directory
*****************************************/
bool InsertXML(char *dirPath);

/*****************************************
 * func: 从源码目录中将所有源文件转化为对应的xml文件
 * return: void
******************************************/
bool BuildSrcToXml();

/*****************************************
 * func: 将所有xml文件转为为对应的源码文件
 * return: void
******************************************/
bool BuildXmlToSrc();

/*****************************************
 * func: 将所有xml文件进行插装
 * return: void
******************************************/
bool BuildInsertXml();

/*****************************************
 * func: 删除临时生产的文件
 * return: void
*****************************************/
void ClearTmp();

#endif