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
#include "sourceOper.h"

#define JudgeVarUsed(cur, var)  JudgeVarUsedFromNode(cur, var, true)
#define JudgeExistChildNode(cur, nodeName)   JudgeExistChildNodeFromNode(cur, nodeName, true)
#define ExtractVarDef(cur)  ExtractVarDefFromNode(cur, true)
#define ExtractVarType(cur)   ExtractVarTypeFromNode(cur, true)
#define scanAssignVar(cur)   scanAssignVarFromNode(cur, true)
#define ExtractDirectInfluVar(cur, varName, varTypeBegin)  ExtractDirectInfluVarFromNode(cur, varName, varTypeBegin, true)
#define ScliceInflVar(varName, cur, varTypeBegin)  ScliceInflVarInfo(varName, cur, NULL, NULL, varTypeBegin)
#define getVarDefValue(varName, cur)  getVarDefValueFromNode(varName, cur, true)

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

/*********************************
 * func: Extract all global variable define info from a xml file
 * return: void
 * @para xmlFilePath: xml file path
*********************************/
void ExtractGlobalVarDef(char *xmlFilePath);

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

/********************************
 * func: get current node min line number
 * return: NULL = no line
 * @para cur: current node
********************************/
xmlChar *getLine(xmlNodePtr cur);

/**********************************
 * func: sclice influence variable
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para cur: current analyze node
 * @para inflVarName: influenced variable name
 * @para curInflVar: current influenced variable list
 * @para varTypeBegin: current node self-define vaiable info
**********************************/
varDef *ScliceInflVarInfo(char *varName, xmlNodePtr cur, char *inflVarName, varDef *curInflVar, varType *varTypeBegin);

/**********************************
 * func: variable sclice return influence function
 * return: not null = exist influence   null = not exist influence
 * @para varInfo: analyse variable info
 * @para xmlFilePath: xml file path
 * @para varScliceFunc: C or C++ variable Sclice function point
**********************************/
funcInfo *Sclice(char *varName, char *xmlFilePath, funcCallList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool));

/**********************************
 * func: variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varInfo: analyse variable info
 * @para xmlFilePath: xml file path
 * @para varScliceFunc: C or C++ variable Sclice function point
**********************************/
funcInfo *ScliceDebug(char *varName, char *xmlFilePath, funcCallList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool));

/***********************************
 * func: judge expr node whether is varName default value templet or not
 * return: confVarDefValue.defValue = -1 : no default value
 * @para varName: variable name
 * @para expr: expression node
************************************/
confVarDefValue judgeVarDefValueModel(char *varName, xmlNodePtr expr);

/**********************************
 * func: get variable default value from current Node
 * return: default value  confVarDefValue.defValue = -1 : no default value
 * @para varName: variable name
 * @para cur: function block node
***********************************/
confVarDefValue getVarDefValueFromNode(char *varName, xmlNodePtr funcNode, bool flag);

/***********************************
 * func: get function parameter name by parameter position
 * return: parameter name    NULL = get failure
 * @para parameterListNode: parameter list node 
 * @para index: parameter position
***********************************/
char *getParaNameByIndex(xmlNodePtr parameterListNode, int index);

/**************************************
 * func: get the position of specific argument position
 * return: -1 = not finded parameter
 * @para paraName: finded parameter name
 * @para paraListNode: parameter list node
***************************************/
int getArguPosition(char *paraName, xmlNodePtr paraListNode);


/**********************************
 * func: extract specific parameter default value
 * return: confVarDefValue.defValue = -1 : no default value
 * @para paraIndex: parameter position
 * @para funcName: the function name in which the variable used
 * @para xmlFilePath: the file path in which the function is located
***********************************/
confVarDefValue ExtractSpeciParaDefValue(int paraIndex, char *funcName, char *xmlFilePath, \
    varDirectInflFunc *(*DirectInflFunc)(char *, xmlNodePtr, varType *, bool));

#endif