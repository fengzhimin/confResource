/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : buildFuncLibrary.h
* Descripe     : build library about relationship between function and system resource
******************************************************/

#ifndef __BUILDFUNCLIBRARY_H__
#define __BUILDFUNCLIBRARY_H__

#include "fileOper.h"
#include "strOper.h"
#include "mysqlOper.h"
#include "logOper.h"
#include <string.h>
#include <errno.h>

/***************************************
 * func: build the relationship library between function and resource.
 * return: true = success    false = failure
****************************************/
bool buildLibrary();

#endif