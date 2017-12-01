/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : JsonOper.h
* Descripe     : json file operation
******************************************************/

#ifndef __JSONOPER_H__
#define __JSONOPER_H__

#include "config.h"
#include "strOper.h"
#include "fileOper.h"
#include "logOper.h"
#include <errno.h>

/***************************************
 * func: get software config info(JsonPath)
 * return: true = success   false = failure
****************************************/
bool getSoftWareConfInfo();

/*****************************************
 * func: 从json文件中提取内容
 * return: PreCompileInfoList列表
 * @para jsonPath: json文件路径
******************************************/
PreCompileInfoList *ExtractInfoFromJson();

#endif