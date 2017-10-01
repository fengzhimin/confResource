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
    sprintf(classInheritTableName, "%s_classInheritTable", programName);
    replaceChar(classInheritTableName, '-', '_');
    replaceChar(classInheritTableName, '.', '_');
    sprintf(funcCallTableName, "%s_funcCall", programName);
    replaceChar(funcCallTableName, '-', '_');
    replaceChar(funcCallTableName, '.', '_');
    int i;
    for(i = 0; i < MAX_ANALYZE_XML_PTHREAD_NUM; i++)
    {
        memset(tempFuncScoreTableName[i], 0, MAX_PROGRAMNAME_NUM*2*sizeof(char));
        sprintf(tempFuncScoreTableName[i], "temp%d_%s_funcScore", i+1, programName);
        replaceChar(tempFuncScoreTableName[i], '-', '_');
        replaceChar(tempFuncScoreTableName[i], '.', '_');
        memset(tempFuncCallTableName[i], 0, MAX_PROGRAMNAME_NUM*2*sizeof(char));
        sprintf(tempFuncCallTableName[i], "temp%d_%s_funcCall", i+1, programName);
        replaceChar(tempFuncCallTableName[i], '-', '_');
        replaceChar(tempFuncCallTableName[i], '.', '_');
    }
    
    
    //create temp table command
    sprintf(createFuncScoreTable, createFuncScoreTableTemplate, funcScoreTableName);
    sprintf(createClassInheritTable, createClassInheritTableTemplate, classInheritTableName);
    sprintf(createFuncCallTable, createFuncCallTableTemplate, funcCallTableName);
    
    sprintf(deleteFuncScoreTable, deleteFuncScoreTableTemplate, funcScoreTableName);
    sprintf(deleteClassInheritTable, deleteClassInheritTableTemplate, classInheritTableName);
    sprintf(deleteFuncCallTable, deleteFuncCallTableTemplate, funcCallTableName);
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
    int i;
    for(i = 0; i < MAX_ANALYZE_XML_PTHREAD_NUM; i++)
    {
        sprintf(createTempFuncScoreTable, createTempFuncScoreTableTemplate, tempFuncScoreTableName[i]);
        if(executeCommand(createTempFuncScoreTable))
            ret = true;
        else
        {
            RecordLog("create tempFuncScore table failure!\n");
            ret = false;
        }
        sprintf(createTempFuncCallTable, createTempFuncCallTableTemplate, tempFuncCallTableName[i]);
        if(executeCommand(createTempFuncCallTable))
        ret = true;
        else
        {
            RecordLog("create tempFuncCall table failure!\n");
            ret = false;
        }
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
    int i;
    for(i = 0; i < MAX_ANALYZE_XML_PTHREAD_NUM; i++)
    {
        sprintf(deleteTempFuncScoreTable, deleteTempFuncScoreTableTemplate, tempFuncScoreTableName[i]);
        if(executeCommand(deleteTempFuncScoreTable))
            ret = true;
        else
        {
            RecordLog("delete tempFuncScore table failure!\n");
            ret = false;
        }
        sprintf(deleteTempFuncCallTable, deleteTempFuncCallTableTemplate, tempFuncCallTableName[i]);
        if(executeCommand(deleteTempFuncCallTable))
        ret = true;
        else
        {
            RecordLog("delete tempFuncCall table failure!\n");
            ret = false;
        }
    }
    
    return ret;
}
