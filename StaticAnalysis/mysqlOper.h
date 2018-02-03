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

#define executeCommand(command)  executeSQLCommand(mysqlConnect, command)

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
 * @para mysqlConn: NULL = create temp connect   no NULL = use global mysqlConnect
 * @para command: command string
*******************************/
bool executeSQLCommand(MYSQL *mysqlConn, char *command);

#endif