/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : mysqlOper.h
* Descripe     : operate mysql database
******************************************************/

#ifndef __MYSQLOPER_H__
#define __MYSQLOPER_H__

#include <stdio.h>
#include <string.h>
#include "config.h"
#include "logOper.h"
#include "confOper.h"

/******************************
 * func: initial mysql operation
 * return: true = success   false = failure
*******************************/
bool startMysql();

/******************************
 * func: release mysql resource
 * return: void
******************************/
void stopMysql();

/******************************
 * func: execute a command
 * bool: true = success false = failure
 * @para command string
*******************************/
bool executeCommand(char *command);

#endif