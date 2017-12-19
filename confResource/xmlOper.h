/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : xmlOper.h
* Descripe     : parse xml file
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
#include "CXmlOper.h"
#include "CPPXmlOper.h"
#include "searchLoopCount.h"

/*******************************
 * func: Extract function Name from XML file
 * return: true = success    false = failure
 * @para xmlFilePath: xml path
 * @para tempFuncScoreTableName: temporary funcScore table name
 * @para tempFuncCallTableName: temporary funcCall table name
********************************/
bool ExtractFuncFromXML(char *xmlFilePath, char *tempFuncScoreTableName, char *tempFuncCallTableName);

/**********************************
 * func: variable sclice
 * return: not null = exist influence   null = not exist influence
 * @para varName: variable name
 * @para xmlFilePath: xml file path
**********************************/
funcCallInfoList *VarSclice(char *varName, char *xmlFilePath);

/**********************************
 * func: 判断一个变量是否通过数据传播影响给定的一条函数调用路径(目的是为了判断配置项变量是否应该库函数，从而确定库函数的基础分数)
 * return: true = 影响   false = 不影响
 * @para varName: 分析的变量
 * @para funcCallPath: 函数调用路径
 * @para score: 当前分析的函数中各类资源的分数，如果配置项变量影响了该函数中的库函数，则score返回的值为配置项默认值
***********************************/
bool JudgeVarInflFuncCallPath(char *varName, funcInfoList *funcCallPath, confScore *score);

/**********************************
 * func: 判断一个变量是否通过数据传播影响给定的一条函数调用路径上最后一个函数中指定的变量(判断配置项变量是否影响指定的变量influedVarName)
 * return: true = 影响   false = 不影响
 * @para varName: 分析的变量
 * @para influedVarName: 判断被影响的变量
 * @para funcCallPath: 函数调用路径
***********************************/
bool JudgeVarInflSpeciVarByFuncCallPath(char *varName, char *influedVarName, funcInfoList *funcCallPath);

/***********************************
 * func: 提取指定变量通过数据传播影响给定的一条函数调用路径上最后一个函数的变量
 * return: 影响变量列表
 * @para varName: 分析的变量
 * @para funcCallPath: 函数调用路径
***********************************/
varDef *ExtractVarInflVarByFuncCallPath(char *varName, funcInfoList *funcCallPath);

/*************************************
 * func: 获取一条函数调用路径上的循环倍数关系，为了得出该配置项变量所影响资源的关系
 * return: 各资源对应的系数
 * @para varName: 分析的变量
 * @para funcCallPath: 函数调用路径
**************************************/
relationExpr getConfOptRelation(char *varName, funcInfoList *funcCallPath);

#endif