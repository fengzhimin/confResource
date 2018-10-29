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
#include "sourceOper.h"
#include "CXmlOper.h"
#include "CPPXmlOper.h"
#include "JavaXmlOper.h"

/**********************************
 * func: 向一个源文件中插入记录循环次数的代码
 * return: true = success   false = failure
 * @para handledConfig: 需要被处理的配置项信息
**********************************/
bool InsertMarkerCode(HandledConf handledConfig);

#endif