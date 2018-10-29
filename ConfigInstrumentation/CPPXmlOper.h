/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CPPXmlOper.h
* Descripe     : parse C++ language xml file
******************************************************/

#ifndef __CPPXMLOPER_H__
#define __CPPXMLOPER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "config.h"
#include "logOper.h"
#include "strOper.h"

/********************************
 * func: 添加头文件
 * return: 
 * @para root_node: xml根节点
********************************/
void AddCPPHeaderFile(xmlNodePtr root_node);

/********************************
 * func: 查找函数定义的节点
 * return: 函数funcName的节点   NULL = failure
 * @para root_node: xml根节点
 * @para funcName:　需要查找的函数名
*********************************/
xmlNodePtr getCPPFuncBlockNodeByFuncName(xmlNodePtr root_node, char *funcName);

/*********************************
 * func: 向一个函数中插入标记代码来标记配置项是否触发
 * return: 
 * @para funcBlockNode: 函数体节点
 * @para confName: 配置项名
*********************************/
void AddCPPMarkerCode(xmlNodePtr funcBlockNode, char *confName);

#endif