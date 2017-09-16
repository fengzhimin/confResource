/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : buildTempTable.c
* Descripe     : build funcScore table and funcCall table
******************************************************/

#include "buildTempTable.h"

static char createFuncScoreTable[1024] = "";
static char createTempFuncScoreTable[1024] = "";
static char createClassInheritTable[1024] = "";
static char createFuncCallTable[1024] = "";
static char createTempFuncCallTable[1024] = "";
static char deleteFuncScoreTable[1024] = "";
static char deleteTempFuncScoreTable[1024] = "";
static char deleteClassInheritTable[1024] = "";
static char deleteFuncCallTable[1024] = "";
static char deleteTempFuncCallTable[1024] = "";

void initTableName()
{
    //set temp table name
    sprintf(funcScoreTableName, "%s_funcScore", programName);
    replaceChar(funcScoreTableName, '-', '_');
    replaceChar(funcScoreTableName, '.', '_');
    sprintf(tempFuncScoreTableName, "temp_%s_funcScore", programName);
    replaceChar(tempFuncScoreTableName, '-', '_');
    replaceChar(tempFuncScoreTableName, '.', '_');
    sprintf(classInheritTableName, "%s_classInheritTable", programName);
    replaceChar(classInheritTableName, '-', '_');
    replaceChar(classInheritTableName, '.', '_');
    sprintf(funcCallTableName, "%s_funcCall", programName);
    replaceChar(funcCallTableName, '-', '_');
    replaceChar(funcCallTableName, '.', '_');
    sprintf(tempFuncCallTableName, "temp_%s_funcCall", programName);
    replaceChar(tempFuncCallTableName, '-', '_');
    replaceChar(tempFuncCallTableName, '.', '_');
    
    //create temp table command
    sprintf(createFuncScoreTable, createFuncScoreTableTemplate, funcScoreTableName);
    sprintf(createTempFuncScoreTable, createTempFuncScoreTableTemplate, tempFuncScoreTableName);
    sprintf(createClassInheritTable, createClassInheritTableTemplate, classInheritTableName);
    sprintf(createFuncCallTable, createFuncCallTableTemplate, funcCallTableName);
    sprintf(createTempFuncCallTable, createTempFuncCallTableTemplate, tempFuncCallTableName);
    
    sprintf(deleteFuncScoreTable, deleteFuncScoreTableTemplate, funcScoreTableName);
    sprintf(deleteTempFuncScoreTable, deleteTempFuncScoreTableTemplate, tempFuncScoreTableName);
    sprintf(deleteClassInheritTable, deleteClassInheritTableTemplate, classInheritTableName);
    sprintf(deleteFuncCallTable, deleteFuncCallTableTemplate, funcCallTableName);
    sprintf(deleteTempFuncCallTable, deleteTempFuncCallTableTemplate, tempFuncCallTableName);
}

bool buildTempTable()
{
    //delete already exist temp table
    bool ret = deleteTempTable();

    //create function score table
    if(executeCommand(createFuncScoreTable))
        ret = true;
    else
    {
        RecordLog("create funcScore table failure!\n");
        ret = false;
    }
    if(executeCommand(createTempFuncScoreTable))
        ret = true;
    else
    {
        RecordLog("create tempFuncScore table failure!\n");
        ret = false;
    }
    
    //create class inherit table
    if(executeCommand(createClassInheritTable))
        ret = true;
    else
    {
        RecordLog("create class inherit table failure!\n");
        ret = false;
    }
    
    //create function call table
    if(executeCommand(createFuncCallTable))
        ret = true;
    else
    {
        RecordLog("create funcCall table failure!\n");
        ret = false;
    }
    if(executeCommand(createTempFuncCallTable))
        ret = true;
    else
    {
        RecordLog("create tempFuncCall table failure!\n");
        ret = false;
    }
    
    return ret;
}

bool deleteTempTable()
{
    bool ret = false;

    //create function score table
    if(executeCommand(deleteFuncScoreTable))
        ret = true;
    else
    {
        RecordLog("delete funcScore table failure!\n");
        ret = false;
    }
    if(executeCommand(deleteTempFuncScoreTable))
        ret = true;
    else
    {
        RecordLog("delete tempFuncScore table failure!\n");
        ret = false;
    }
    
    //create class inherit table
    if(executeCommand(deleteClassInheritTable))
        ret = true;
    else
    {
        RecordLog("delete class inherit table failure!\n");
        ret = false;
    }
    
    //create function call table
    if(executeCommand(deleteFuncCallTable))
        ret = true;
    else
    {
        RecordLog("delete funcCall table failure!\n");
        ret = false;
    }
    if(executeCommand(deleteTempFuncCallTable))
        ret = true;
    else
    {
        RecordLog("delete tempFuncCall table failure!\n");
        ret = false;
    }
    
    return ret;
}
