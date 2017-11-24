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
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "config.h"
#include "strOper.h"

/********************************
 * func: 添加头文件
 * return: 
 * @para root_node: xml根节点
********************************/
void AddHeaderFile(xmlNodePtr root_node);

/*********************************
 * func: 向一个函数中的循环添加count记录
 * return: 
 * @para funcBlockNode: 函数体节点
 * @para funcName: 函数名
 * @para srcPath: 函数所在源文件
*********************************/
void AddLoopCount(xmlNodePtr funcBlockNode, char *funcName, char *srcPath, int *loopCount);

/********************************
 * func: 查找函数定义的节点
 * return: SelfDefFuncNodeList 列表首地址
 * @para root_node: xml根节点
*********************************/
SelfDefFuncNodeList *getFuncBlockNodeAndName(xmlNodePtr root_node);

/**********************************
 * func: 向一个源文件中插入记录循环次数的代码
 * return: true = success   false = failure
 * @para filePath: 源文件路径
**********************************/
bool InsertCode(char *filePath);

#endif