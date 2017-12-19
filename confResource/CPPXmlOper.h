/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CPPXmlOper.h
* Descripe     : parse C Plus Plus language xml file
******************************************************/

#ifndef __CPPXMLOPER_H__
#define __CPPXMLOPER_H__

#include <stdio.h> 
#include <string.h>  
#include <stdlib.h>
#include <stdbool.h>
#include <libxml/xmlmemory.h>  
#include <libxml/parser.h>
#include "logOper.h"
#include "config.h"
#include "mysqlOper.h"
#include "commonXmlOper.h"

#define varCPPScliceFunc(varInfo, cur, varTypeBegin)   varCPPScliceFuncFromNode(varInfo, cur, varTypeBegin, true)
#define scanCPPCallFunc(cur, varTypeBegin)    scanCPPCallFuncFromNode(cur, varTypeBegin, true)
#define getCPPDirectInflFunc(varName, funcBlockNode, varTypeBegin)  getCPPDirectInflFuncFromNode(varName, funcBlockNode, varTypeBegin, true)

/*******************************
 * func: Extract function Name from C Plus Plus language XML file
 * return: true = success    false = failure
 * @para xmlFilePath: xml path
 * @para tempFuncScoreTableName: temporary funcScore table name
 * @para tempFuncCallTableName: temporary funcCall table name
********************************/
bool ExtractFuncFromCPPXML(char *xmlFilePath, char *tempFuncScoreTableName, char *tempFuncCallTableName);

/*******************************
 * func: Extract C++ function Name from current node
 * return: true = success    false = failure
 * @para xmlFilePath: xml path
 * @para cur: current node
 * @para tempFuncScoreTableName: temporary funcScore table name
 * @para tempFuncCallTableName: temporary funcCall table name
********************************/
bool ExtractCPPFunc(char *xmlFilePath, xmlNodePtr cur, char *tempFuncScoreTableName, char *tempFuncCallTableName);

/*******************************
 * func: scan call function from current node
 * return: call function list header point
 * @para cur: current  Node
********************************/
funcInfoList *scanCPPCallFuncFromNode(xmlNodePtr cur, varType *varTypeBegin, bool flag);

/*******************************
 * func: scan called function from current node back node
 * return: call function list header point
 * @para cur: current Node
*******************************/
funcInfoList *scanBackCPPCallFunc(xmlNodePtr cur, varType *varTypeBegin);

/*******************************
 * func: Extract class inherit from C Plus Plus language XML file
 * return: true = success    false = failure
 * @para docName: xml path
********************************/
bool ExtractClassInheritFromCPPXML(char *docName);

/*********************************
 * func: sclice variable influence function call
 * return: influence call function header point
 * @para varInfo: analyse variable info
 * @para cur: current node
**********************************/
funcInfoList *varCPPScliceFuncFromNode(varDef varInfo, xmlNodePtr cur, varType *varTypeBegin, bool flag);

/**********************************
 * func: C++ language variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
funcCallInfoList *CPPSclice(char *varName, char *xmlFilePath);

/************************************
 * func: get C++ language variable direct influence function information
 * return: influenced function info list
 * @para varName: analysed variable name
 * @para funcBlockNode: analysed function block node
 * @para varTypeBegin: analysed funcBlockNode variable define info
*************************************/
varDirectInflFuncList *getCPPDirectInflFuncFromNode(char *varName, xmlNodePtr funcBlockNode, varType *varTypeBegin, bool flag);

/***********************************
 * func: 获取变量varName在C++函数funcName中通过数据传播所影响的被调用的函数信息
 * return: 影响函数的列表
 * @para varName: 要分析的变量
 * @para funcName: 要分析的函数名
 * @para xmlFilePath: funcName函数所在的xml文件路径
 * @para funcArgumentType: funcName函数的参数格式
***********************************/
varDirectInflFuncList *getCPPVarInfluFunc(char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType);

#endif