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

#define ExtractVarDef(cur)  ExtractVarDefFromNode(cur, true) 

/*******************************
 * func: Extract function Name from XML file
 * return: true = success    false = failure
 * @para docName: xml path
********************************/
bool ExtractFuncFromXML(char *docName);

/*******************************
 * func: self-define function scan call function
 * return: void
 * @para cur: current self-define function Node
 * @para funcName: current self-define function name
 * @para funcType: current self-define function type(extern or static)
 * @para srcPath: function source file path
********************************/
void scanCallFunction(xmlNodePtr cur, char *funcName, char *funcType, char *srcPath);

/*******************************
 * func: scan call function
 * return: void
 * @para cur: current  Node
********************************/
void scanCallFunc(xmlNodePtr cur);

/*******************************
 * func: extract variable used function from a xml file(function and line)
 * return: save use variable funcName    NULL = xmlFilePath not exist use varName
 * @para varName: variable name
 * @para xmlFilePath: xml style file path
********************************/
funcList *ExtractVarUsedFunc(char *varName, char *xmlFilePath);

/********************************
 * func: judge current node whether use variable or not
 * return: true = use   false = not use
 * @para cur: current node
 * @para varName: variable name
********************************/
bool scanVarIsUsed(xmlNodePtr cur, char *varName);

/*********************************
 * func: Extract variable define info
 * return: varType list
 * @para cur: current node
 * @para flag: wether scan next node or not
*********************************/
varType *ExtractVarDefFromNode(xmlNodePtr cur, bool flag);

/*********************************
 * func: Extract variable used info
 * return: void
 * @para cur: current node
**********************************/
void ExtractVarUsedInfo(xmlNodePtr cur);

/*********************************
 * func: Extract function all variable define info from a xml file
 * return: void
 * @para xmlFilePath: xml file path
*********************************/
void ExtractFuncVarDef(char *xmlFilePath);

/*********************************
 * func: Extract all global variable define info from a xml file
 * return: void
 * @para xmlFilePath: xml file path
*********************************/
void ExtractGlobalVarDef(char *xmlFilePath);

/*********************************
 * func: Extract function all variable used info from a xml file
 * return: void
 * @para xmlFilePath: xml file path
*********************************/
void ExtractFuncVarUsedInfo(char *xmlFilePath);

/*********************************
 * return: void
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
void varSclice(char *varName, xmlNodePtr cur);

/**********************************
 * func: variable slice
 * return: void
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
void Sclice(char *varName, char *xmlFilePath);

#endif