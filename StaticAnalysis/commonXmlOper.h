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
#define ExtractErrorVarType(cur)   ExtractErrorVarTypeFromNode(cur, true)
#define ExtractVarType(cur)   ExtractVarTypeFromNode(cur, true)
#define scanAssignVar(cur)   scanAssignVarFromNode(cur, true)
#define ExtractDirectInfluVar(cur, varName, varTypeBegin)  ExtractDirectInfluVarFromNode(cur, varName, varTypeBegin, true)
#define ScliceInflVar(varName, cur, varTypeBegin)  ScliceInflVarInfo(varName, cur, NULL, NULL, varTypeBegin)

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

/**********************************
 * func: compare function argument format whether similar or not
 * return: true = similar
 * @para funcName: function name
 * @para xmlFilePath: funcName 所在的xml文件路径
 * @para arg1: first argument type string
 * @para arg2: second argument type string
***********************************/
bool JudgeArgumentSimilar(char *funcName, char *xmlFilePath, char *arg1, char *arg2);

/*********************************
 * func: Extract variable define info
 * return: varType list
 * @para cur: current node
 * @para flag: whether scan next node or not
*********************************/
varType *ExtractVarDefFromNode(xmlNodePtr cur, bool flag);

/*********************************
 * func: 提取解析错误函数的变量定义
 * return: varType list
 * @para cur: current node
 * @para flag: whether scan next node or not
*********************************/
varType *ExtractErrorVarTypeFromNode(xmlNodePtr cur, bool flag);

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
 * func: 提取错误参数类型
 * return: argument type string
 * @para cur: current function node
 * @example: void fun(void) ---> void
 * @example: void fun(int a, const char *str)   ---> (int/char#2)
*******************************/
char *ExtractErrorFuncArgumentType(xmlNodePtr cur);

/*******************************
 * func: extract function argument type
 * return: argument type string
 * @para cur: current function node
 * @example: void fun(void) ---> void
 * @example: void fun(int a, const char *str)   ---> (int/char#2)
*******************************/
char *ExtractFuncArgumentType(xmlNodePtr cur);

/*******************************
 * func: get called function argument type
 * return: argument type string
 * @para cur: called function node
 * @para funcDefVarType: current function all define variable type list
 * @example: int a
 *           func(a)  ---> (int#1)
*******************************/
char *getCalledFuncArgumentType(xmlNodePtr cur, varType *funcDefVarType);

/******************************
 * func: get function name
 * return: function name
 * @para funcNode: function children node
******************************/
char *getFuncName(xmlNodePtr funcNode);

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
 * func: 处理解析错误的程序(sql_connect.cc中的get_key_conn)
 * return: not null = exist influence   null = not exist influence
 * @para varInfo: analyse variable info
 * @para xmlFilePath: xml file path
 * @para cur: current node
 * @para varScliceFunc: C or C++ variable Sclice function point
**********************************/
funcCallInfoList *ScliceErrorFromNode(char *varName, char *xmlFilePath, xmlNodePtr cur, funcInfoList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool));

/**********************************
 * func: variable sclice return influence function from node
 * return: not null = exist influence   null = not exist influence
 * @para varInfo: analyse variable info
 * @para xmlFilePath: xml file path
 * @para cur: current node
 * @para varScliceFunc: C or C++ variable Sclice function point
**********************************/
funcCallInfoList *ScliceFromNode(char *varName, char *xmlFilePath, xmlNodePtr cur, funcInfoList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool));

/**********************************
 * func: variable sclice return influence function
 * return: not null = exist influence   null = not exist influence
 * @para varInfo: analyse variable info
 * @para xmlFilePath: xml file path
 * @para varScliceFunc: C or C++ variable Sclice function point
**********************************/
funcCallInfoList *Sclice(char *varName, char *xmlFilePath, funcInfoList *(*varScliceFunc)(varDef, xmlNodePtr , varType *, bool));

/***********************************
 * func: get function parameter name by parameter position
 * return: parameter name    NULL = get failure
 * @para parameterListNode: parameter list node 
 * @para index: parameter position
***********************************/
char *getParaNameByIndexFromParaList(xmlNodePtr parameterListNode, int index);

/************************************
 * func: get function parameter name by parameter position
 * return: parameter name    NULL = get failure
 * @para cur: current node
 * @para index: parameter position
 * @para funcName: function name
 * @para xmlFilePath: the file path in which the function is located
 * @para funcArgumentType: the argument type of the funcName
*************************************/
char *getErrorParaNameByIndexFromNode(xmlNodePtr cur, int index, char *funcName, char *xmlFilePath, char *funcArgumentType);

/************************************
 * func: get function parameter name by parameter position
 * return: parameter name    NULL = get failure
 * @para cur: current node
 * @para index: parameter position
 * @para funcName: function name
 * @para xmlFilePath: the file path in which the function is located
 * @para funcArgumentType: the argument type of the funcName
*************************************/
char *getParaNameByIndexFromNode(xmlNodePtr cur, int index, char *funcName, char *xmlFilePath, char *funcArgumentType);

/************************************
 * func: get function parameter name by parameter position
 * return: parameter name    NULL = get failure
 * @para index: parameter position
 * @para funcName: function name
 * @para xmlFilePath: the file path in which the function is located
 * @para funcArgumentType: the argument type of the funcName
*************************************/
char *getParaNameByIndex(int index, char *funcName, char *xmlFilePath, char *funcArgumentType);

/**************************************
 * func: get the position of specific argument position
 * return: -1 = not finded parameter
 * @para paraName: finded parameter name
 * @para paraListNode: parameter list node
***************************************/
int getArguPosition(char *paraName, xmlNodePtr paraListNode);

/***********************************
 * func: 获取变量varName在函数funcName中通过数据传播所影响的被调用的函数信息
 * return: 影响函数的列表
 * @para cur: current node
 * @para varName: 要分析的变量
 * @para funcName: 要分析的函数名
 * @para xmlFilePath: funcName函数所在的xml文件路径
 * @para funcArgumentType: funcName函数的参数格式
***********************************/
varDirectInflFuncList *getErrorVarInfluFuncFromNode(xmlNodePtr cur, char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType, \
    varDirectInflFuncList *(*DirectInflFunc)(char *, xmlNodePtr, varType *, bool));
    
/***********************************
 * func: 获取变量varName在函数funcName中通过数据传播所影响的被调用的函数信息
 * return: 影响函数的列表
 * @para cur: current node
 * @para varName: 要分析的变量
 * @para funcName: 要分析的函数名
 * @para xmlFilePath: funcName函数所在的xml文件路径
 * @para funcArgumentType: funcName函数的参数格式
***********************************/
varDirectInflFuncList *getVarInfluFuncFromNode(xmlNodePtr cur, char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType, \
    varDirectInflFuncList *(*DirectInflFunc)(char *, xmlNodePtr, varType *, bool));

/***********************************
 * func: 获取变量varName在函数funcName中通过数据传播所影响的被调用的函数信息
 * return: 影响函数的列表
 * @para varName: 要分析的变量
 * @para funcName: 要分析的函数名
 * @para xmlFilePath: funcName函数所在的xml文件路径
 * @para funcArgumentType: funcName函数的参数格式
***********************************/
varDirectInflFuncList *getVarInfluFunc(char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType, \
    varDirectInflFuncList *(*DirectInflFunc)(char *, xmlNodePtr, varType *, bool));
    
/************************************
 * func: 获取变量varName在函数funcName中通过数据传播所影响的变量信息
 * return: 被影响的变量信息
 * @para varName: 要分析的变量
 * @para funcName: 要分析的函数名
 * @para xmlFilePath: funcName函数所在的xml文件路径
 * @para funcArgumentType: funcName函数的参数格式
***********************************/
varDef *getVarInfluVarInfo(char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType);

/*************************************
 * func: get specific called function node from current node
 * return: called function node
 * @para cur: current node
 * @para calledFuncName: called function name
 * @para calledLine: called line
**************************************/
xmlNodePtr getSpeciCalledFuncNode(xmlNodePtr cur, char *calledFuncName, int calledLine);

/**************************************
 * func: 获取一个被调用函数所处的循环信息表达式
 * return: 返回所处循环信息的列表
 * @para funcName: 调用函数的名称
 * @para xmlFilePath: funcName所在xml路径
 * @para funcArgumentType: funcName 参数类型
 * @para calledFuncName: 被调用函数名称
 * @para calledLine: calledFuncName在函数funcName中被调用的行数
***************************************/
loopExprList *getCalledFuncLoopInfo(char *funcName, char *xmlFilePath, char *funcArgumentType, char *calledFuncName, int calledLine);

/**************************************
 * func: 获取一个被调用函数所处的循环记录变量信息
 * return: 返回记录循环次数信息的列表
 * @para funcName: 调用函数的名称
 * @para xmlFilePath: funcName所在xml路径
 * @para funcArgumentType: funcName 参数类型
 * @para calledFuncName: 被调用函数名称
 * @para calledLine: calledFuncName在函数funcName中被调用的行数
***************************************/
loopCountInfoList *getCalledFuncLoopCountInfo(char *funcName, char *xmlFilePath, char *funcArgumentType, char *calledFuncName, int calledLine);

#endif
