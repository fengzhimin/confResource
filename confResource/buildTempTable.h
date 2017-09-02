/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : buildTempTable.h
* Descripe     : build funcScore table and funcCall table
******************************************************/

#ifndef __BUILDTEMPTABLE_H__
#define __BUILDTEMPTABLE_H__

#include "mysqlOper.h"
#include "logOper.h"
#include "config.h"

/***************************************
 * func: build temp table(funcScoreTable and funcCallTable)
 * return: true = success    false = failure
****************************************/
bool buildTempTable();

/***************************************
 * func: delete temp table(funcScoreTable and funcCallTable)
 * return: true = success    false = failure
***************************************/
bool deleteTempTable();

#endif