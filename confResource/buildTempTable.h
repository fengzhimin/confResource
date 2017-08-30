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