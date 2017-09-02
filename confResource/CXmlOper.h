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

/*******************************
 * func: Extract function Name from XML
 * return: true = success    false = failure
 * @para docName: xml path
********************************/
bool ExtractFuncFromXML(char *docName);

/*******************************
 * func: scan call function
 * return: void
 * @para cur: current Node
 * @para funcName: current self-define function name
 * @para srcPath: function source file path
********************************/
void scanCallFunction(xmlNodePtr cur, char *funcName, char *srcPath);

/*******************************
 * func: extract variable use position from a xml file(function and line)
 * return: save use variable funcName    NULL = xmlFilePath not exist use varName
 * @para varName: variable name
 * @para xmlFilePath: xml style file path
********************************/
funcList *ExtractVarUsedPos(char *varName, char *xmlFilePath);

/********************************
 * func: judge current node whether use variable or not
 * return: true = use   false = not use
 * @para cur: current node
 * @para varName: variable name
********************************/
bool scanVarIsUsed(xmlNodePtr cur, char *varName);

#endif