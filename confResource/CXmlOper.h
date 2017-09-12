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

#define scanCallFunc(cur)    scanCallFuncFromNode(cur, true)
#define scanAssignVar(cur)   scanAssignVarFromNode(cur, true)
#define scanVarIsUsed(cur, varName)    scanVarIsUsedFromNode(cur, varName, true)
#define ExtractVarDef(cur)  ExtractVarDefFromNode(cur, true)
#define ExtractVarUsedInfo(cur)    ExtractVarUsedInfoFromNode(cur, true)
#define JudgeVarUsed(cur, var)  JudgeVarUsedFromNode(cur, var, true)
#define JudgeExistChildNode(cur, nodeName)   JudgeExistChildNodeFromNode(cur, nodeName, true)
#define varScliceFunc(varName, cur)   varScliceFuncFromNode(varName, cur, true)
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
funcCallList *scanCallFuncFromNode(xmlNodePtr cur, bool flag);

/*******************************
 * func: scan assignment variable from current node
 * return: void
 * @para cur: current  Node
********************************/
void scanAssignVarFromNode(xmlNodePtr cur, bool flag);

/*******************************
 * func: scan called function from current node back node
 * return: call function list header point
 * @para cur: current Node
*******************************/
funcCallList *scanBackCallFunc(xmlNodePtr cur);

/*******************************
 * func: scan assignment from current node back node
 * return: void
 * @para cur: current Node
*******************************/
void scanBackAssignVar(xmlNodePtr cur);

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
bool scanVarIsUsedFromNode(xmlNodePtr cur, char *varName, bool flag);

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
void ExtractVarUsedInfoFromNode(xmlNodePtr cur, bool flag);

/*********************************
 * func: judge a variable whether be used in current node or not
 * return: true = used    false = not used
 * @para cur: current node
 * @para var: variable
**********************************/
bool JudgeVarUsedFromNode(xmlNodePtr cur, char *var, bool flag);

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

/********************************
 * func: judge a node whether exist specfic node or not
 * return: true = exist    false = not exist
 * @para cur: current node
 * @para nodeName: judged node name
********************************/
bool JudgeExistChildNodeFromNode(xmlNodePtr cur, char *nodeName, bool flag);

/*********************************
 * func: sclice variable influence function call
 * return: influence call function header point
 * @para varName: variable name
 * @para cur: current node
**********************************/
funcCallList *varScliceFuncFromNode(char *varName, xmlNodePtr cur, bool flag);

/**********************************
 * func: variable sclice
 * return: true = exist influence   false = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
funcList *Sclice(char *varName, char *xmlFilePath);

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