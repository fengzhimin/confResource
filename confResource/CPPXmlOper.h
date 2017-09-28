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

#define varCPPScliceFunc(varName, cur, varTypeBegin)   varCPPScliceFuncFromNode(varName, cur, varTypeBegin, true)
#define scanCPPCallFunc(cur, varTypeBegin)    scanCPPCallFuncFromNode(cur, varTypeBegin, true)

/*******************************
 * func: Extract function Name from C Plus Plus language XML file
 * return: true = success    false = failure
 * @para docName: xml path
********************************/
bool ExtractFuncFromCPPXML(char *docName, char *tempFuncScoreTableName, char *tempFuncCallTableName);

/*******************************
 * func: scan call function from current node
 * return: call function list header point
 * @para cur: current  Node
********************************/
funcCallList *scanCPPCallFuncFromNode(xmlNodePtr cur, varType *varTypeBegin, bool flag);

/*******************************
 * func: scan called function from current node back node
 * return: call function list header point
 * @para cur: current Node
*******************************/
funcCallList *scanBackCPPCallFunc(xmlNodePtr cur, varType *varTypeBegin);

/*******************************
 * func: Extract class inherit from C Plus Plus language XML file
 * return: true = success    false = failure
 * @para docName: xml path
********************************/
bool ExtractClassInheritFromCPPXML(char *docName);

/*********************************
 * func: sclice variable influence function call
 * return: influence call function header point
 * @para varName: variable name
 * @para cur: current node
**********************************/
funcCallList *varCPPScliceFuncFromNode(char *varName, xmlNodePtr cur, varType *varTypeBegin, bool flag);

/**********************************
 * func: C++ language variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
funcList *CPPSclice(char *varName, char *xmlFilePath);

#endif