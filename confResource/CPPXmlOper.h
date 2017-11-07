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
 * @para docName: xml path
 * @para tempFuncScoreTableName: temporary funcScore table name
 * @para tempFuncCallTableName: temporary funcCall table name
********************************/
bool ExtractFuncFromCPPXML(char *docName, char *tempFuncScoreTableName, char *tempFuncCallTableName);

/*******************************
 * func: Extract C++ function Name from current node
 * return: true = success    false = failure
 * @para docName: xml path
 * @para cur: current node
 * @para tempFuncScoreTableName: temporary funcScore table name
 * @para tempFuncCallTableName: temporary funcCall table name
********************************/
bool ExtractCPPFunc(char *docName, xmlNodePtr cur, char *tempFuncScoreTableName, char *tempFuncCallTableName);

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
varDirectInflFunc *getCPPDirectInflFuncFromNode(char *varName, xmlNodePtr funcBlockNode, varType *varTypeBegin, bool flag);

/*************************************
 * func: get C++ language variable default value
 * return: confVarDefValue.defValue = -1 : no default value
 * @para varName: analysed variable name
 * @para xmlFilePath: xml file path
**************************************/
confVarDefValue getCPPVarDefaultValue(char *varName, char *xmlFilePath);

#endif