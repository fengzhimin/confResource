/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : xmlOper.h
* Descripe     : parse xml file
******************************************************/

#ifndef __XMLOPER_H__
#define __XMLOPER_H__

#include <stdio.h> 
#include <string.h>  
#include <stdlib.h>
#include <stdbool.h>
#include <libxml/xmlmemory.h>  
#include <libxml/parser.h>
#include "logOper.h"
#include "config.h"
#include "mysqlOper.h"
#include "CXmlOper.h"
#include "CPPXmlOper.h"

/*******************************
 * func: Extract function Name from XML file
 * return: true = success    false = failure
 * @para xmlFilePath: xml path
 * @para tempFuncScoreTableName: temporary funcScore table name
 * @para tempFuncCallTableName: temporary funcCall table name
********************************/
bool ExtractFuncFromXML(char *xmlFilePath, char *tempFuncScoreTableName, char *tempFuncCallTableName);

/**********************************
 * func: variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
funcCallInfoList *VarSclice(char *varName, char *xmlFilePath);

/*************************************
 * func: get C++ language variable default value
 * return: confVarDefValue.defValue = -1 : no default value
 * @para varName: analysed variable name
 * @para xmlFilePath: xml file path
**************************************/
confVarDefValue getVarDefaultValue(char *varName, char *xmlFilePath);

/**********************************
 * func: 判断一个变量是否通过数据传播影响给定的一条函数调用路径
 * return: true = 影响   false = 不影响
 * @para varName: 分析的变量
 * @para funcCallPath: 函数调用路径
***********************************/
bool JudgeVarInflFuncCallPath(char *varName, funcInfoList *funcCallPath);

#endif