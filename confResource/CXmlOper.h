/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CXmlOper.h
* Descripe     : parse C language xml file
******************************************************/

#ifndef __CXMLOPER_H__
#define __CXMLOPER_H__

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

#define scanCCallFunc(cur, varTypeBegin)    scanCCallFuncFromNode(cur, varTypeBegin, true)
#define varCScliceFunc(varInfo, cur)   varCScliceFuncFromNode(varInfo, cur, true)
#define ExtractConfKeyUsedInfo(cur, confKey)   ExtractConfKeyUsedInfoFromNode(cur, confKey, true)
#define literalScliceVar(literalName, cur)   literalScliceVarFromNode(literalName, cur, true)
#define getCDirectInflFunc(varName, funcBlockNode)  getCDirectInflFuncFromNode(varName, funcBlockNode, NULL, true)

/*******************************
 * func: Extract function Name from C language XML file
 * return: true = success    false = failure
 * @para xmlFilePath: xml path
 * @para tempFuncScoreTableName: temporary funcScore table name
 * @para tempFuncCallTableName: temporary funcCall table name
********************************/
bool ExtractFuncFromCXML(char *xmlFilePath, char *tempFuncScoreTableName, char *tempFuncCallTableName);

/*******************************
 * func: scan call function from current node
 * return: call function list header point
 * @para cur: current  Node
********************************/
funcInfoList *scanCCallFuncFromNode(xmlNodePtr cur, varType *varTypeBegin, bool flag);

/*******************************
 * func: scan called function from current node back node
 * return: call function list header point
 * @para cur: current Node
*******************************/
funcInfoList *scanBackCCallFunc(xmlNodePtr cur, varType *varTypeBegin);

/*********************************
 * func: sclice variable influence function call
 * return: influence call function header point
 * @para varInfo: analyse variable info
 * @para cur: current node
**********************************/
funcInfoList *varCScliceFuncFromNode(varDef varInfo, xmlNodePtr cur, varType *varTypeBegin, bool flag);

/**********************************
 * func: C language variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
funcCallInfoList *CSclice(char *varName, char *xmlFilePath);

/************************************
 * func: get C language variable direct influence function information
 * return: influenced function info list
 * @para varName: analysed variable name
 * @para funcBlockNode: analysed function block node
 * @para varTypeBegin: 为了兼容C++处理函数， 此处值为NULL
*************************************/
varDirectInflFuncList *getCDirectInflFuncFromNode(char *varName, xmlNodePtr funcBlockNode, varType *varTypeBegin, bool flag);

/***********************************
 * func: 获取变量varName在C函数funcName中通过数据传播所影响的被调用的函数信息
 * return: 影响函数的列表
 * @para varName: 要分析的变量
 * @para funcName: 要分析的函数名
 * @para xmlFilePath: funcName函数所在的xml文件路径
 * @para funcArgumentType: funcName函数的参数格式
***********************************/
varDirectInflFuncList *getCVarInfluFunc(char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType);
    
/**********************************
 * func: extract configuration key used info in current node
 * return: true = used    false = not used
 * @para cur: current node
 * @para confKey: configuration key name
**********************************/
bool ExtractConfKeyUsedInfoFromNode(xmlNodePtr cur, char *confKey, bool flag);

/*********************************
 * func: sclice constant influence variable
 * return: true = used    false = not used
 * @para literalName: constant name
 * @para cur: current node
**********************************/
bool literalScliceVarFromNode(char *literalName, xmlNodePtr cur, bool flag);

/**********************************
 * func: constant sclice
 * return: true = used    false = not used
 * @para confName: configuration key name
 * @para xmlFilePath: xml file path
**********************************/
bool ScliceConfKey(char *confName, char *xmlFilePath);

#endif