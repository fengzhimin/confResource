/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CXmlOper.h
* Descripe     : parse C language xml file
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
#include "commonXmlOper.h"

#define scanCCallFunc(cur, varTypeBegin)    scanCCallFuncFromNode(cur, varTypeBegin, true)
#define varCScliceFunc(varName, cur)   varCScliceFuncFromNode(varName, cur, true)
#define ExtractConfKeyUsedInfo(cur, confKey)   ExtractConfKeyUsedInfoFromNode(cur, confKey, true)
#define literalScliceVar(literalName, cur)   literalScliceVarFromNode(literalName, cur, true)

/*******************************
 * func: Extract function Name from C language XML file
 * return: true = success    false = failure
 * @para docName: xml path
********************************/
bool ExtractFuncFromCXML(char *docName);

/*******************************
 * func: scan call function from current node
 * return: call function list header point
 * @para cur: current  Node
********************************/
funcCallList *scanCCallFuncFromNode(xmlNodePtr cur, varType *varTypeBegin, bool flag);

/*******************************
 * func: scan called function from current node back node
 * return: call function list header point
 * @para cur: current Node
*******************************/
funcCallList *scanBackCCallFunc(xmlNodePtr cur, varType *varTypeBegin);

/*********************************
 * func: sclice variable influence function call
 * return: influence call function header point
 * @para varName: variable name
 * @para cur: current node
**********************************/
funcCallList *varCScliceFuncFromNode(char *varName, xmlNodePtr cur, varType *varTypeBegin, bool flag);

/**********************************
 * func: C language variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
funcList *CSclice(char *varName, char *xmlFilePath);

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