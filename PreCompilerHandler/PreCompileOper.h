/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : PreCompileOper.h
* Descripe     : execute precompile command operation
******************************************************/

#ifndef __PRECOMPILEOPER_H__
#define __PRECOMPILEOPER_H__

#include "config.h"
#include "JsonOper.h"
#include "logOper.h"
#include <errno.h>
#include <sys/wait.h>

/***************************************
 * func: 执行预编译操作
 * @return: true = success     false = failure
 * @para shellPath: shell的脚本的路径
***************************************/
bool ExecCommand(char *shellPath);

/******************************************************
 * func: 执行预编译指令
 * return: true = success   false = failure
 * @para CommandInfoList: 执行预编译指令的信息(包括执行所在路径和执行命令)
*******************************************************/
bool ExecPreCompile(PreCompileInfoList *CommandInfoList);

#endif