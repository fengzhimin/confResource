/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : commonXmlOper.h
* Descripe     : common operate xml file
******************************************************/

#ifndef __COMMONXMLOPER_H__
#define __COMMONXMLOPER_H__

#include <stdio.h> 
#include <string.h>  
#include <stdlib.h>
#include <stdbool.h>
#include <libxml/xmlmemory.h>  
#include <libxml/parser.h>
#include "logOper.h"
#include "config.h"
#include "mysqlOper.h"

#define JudgeVarUsed(cur, var)  JudgeVarUsedFromNode(cur, var, true)
#define JudgeExistChildNode(cur, nodeName)   JudgeExistChildNodeFromNode(cur, nodeName, true)
#define ExtractVarDef(cur)  ExtractVarDefFromNode(cur, true)
#define ExtractVarType(cur)   ExtractVarTypeFromNode(cur, true)
#define scanVarIsUsed(cur, varName)    scanVarIsUsedFromNode(cur, varName, true)
#define scanAssignVar(cur)   scanAssignVarFromNode(cur, true)
#define ExtractDirectInfluVar(cur, varName, varTypeBegin) ExtractDirectInfluVarFromNode(cur, varName, varTypeBegin, true)

/*********************************
 * func: judge a variable whether be used in current node or not
 * return: true = used    false = not used
 * @para cur: current node
 * @para var: variable
**********************************/
bool JudgeVarUsedFromNode(xmlNodePtr cur, char *var, bool flag);

/********************************
 * func: judge a node whether exist specfic node or not
 * return: true = exist    false = not exist
 * @para cur: current node
 * @para nodeName: judged node name
********************************/
bool JudgeExistChildNodeFromNode(xmlNodePtr cur, char *nodeName, bool flag);

/*********************************
 * func: Extract variable define info
 * return: varType list
 * @para cur: current node
 * @para flag: whether scan next node or not
*********************************/
varType *ExtractVarDefFromNode(xmlNodePtr cur, bool flag);

/*********************************
 * func: Extract variable type
 * return: varType list
 * @para cur: current node
 * @para flag: whether scan next node or not
*********************************/
varType *ExtractVarTypeFromNode(xmlNodePtr cur, bool flag);

/********************************
 * func: judge current node whether use variable or not
 * return: true = use   false = not use
 * @para cur: current node
 * @para varName: variable name
********************************/
bool scanVarIsUsedFromNode(xmlNodePtr cur, char *varName, bool flag);

/*********************************
 * func: Extract function all variable define info from a xml file
 * return: void
 * @para xmlFilePath: xml file path
*********************************/
void ExtractFuncVarDef(char *xmlFilePath);

/*********************************
 * func: Extract function all variable used info from a xml file
 * return: void
 * @para xmlFilePath: xml file path
*********************************/
void ExtractFuncVarUsedInfo(char *xmlFilePath);

/*********************************
 * func: Extract all global variable define info from a xml file
 * return: void
 * @para xmlFilePath: xml file path
*********************************/
void ExtractGlobalVarDef(char *xmlFilePath);

/*******************************
 * func: extract variable used function from a xml file(function and line)
 * return: save use variable funcName    NULL = xmlFilePath not exist use varName
 * @para varName: variable name
 * @para xmlFilePath: xml style file path
********************************/
funcList *ExtractVarUsedFunc(char *varName, char *xmlFilePath);

/*******************************
 * func: scan assignment variable from current node
 * return: void
 * @para cur: current  Node
********************************/
void scanAssignVarFromNode(xmlNodePtr cur, bool flag);

/*******************************
 * func: extract direct influence variable by varName from current node
 * return: variable define info list first point
 * @para cur: current  Node
 * @para varName: influence source
 * @para varTypeBegin: current node self-define vaiable info
 * @example: int a = b varName = 'b' --->  b influence a
********************************/
varDef *ExtractDirectInfluVarFromNode(xmlNodePtr cur, char *varName, varType *varTypeBegin, bool flag);

/*******************************
 * func: scan assignment from current node back node
 * return: void
 * @para cur: current Node
*******************************/
void scanBackAssignVar(xmlNodePtr cur);

/*******************************
 * func: extract function argument type
 * return: argument type string
 * @para cur: current function node
 * @example: void fun(void) ---> void
 * @example: void fun(int a, const char *str)   ---> int/char
*******************************/
char *ExtractFuncArgumentType(xmlNodePtr cur);

/*******************************
 * func: get called function argument type
 * return: argument type string
 * @para cur: called function node
 * @para funcDefVarType: current function all define variable type list
 * @example: int a
 *           func(a)  ---> int
*******************************/
char *getCalledFuncArgumentType(xmlNodePtr cur, varType *funcDefVarType);

/**********************************
 * func: variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
 * @para varScliceFunc: C or C++ variable Sclice function point
**********************************/
funcList *Sclice(char *varName, char *xmlFilePath, funcCallList *(*varScliceFunc)(char *, xmlNodePtr , varType *, bool));

/**********************************
 * func: variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
 * @para varScliceFunc: C or C++ variable Sclice function point
**********************************/
funcList *ScliceDebug(char *varName, char *xmlFilePath, funcCallList *(*varScliceFunc)(char *, xmlNodePtr , varType *, bool));

#endif