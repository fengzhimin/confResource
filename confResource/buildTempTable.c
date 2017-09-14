/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : buildTempTable.c
* Descripe     : build funcScore table and funcCall table
******************************************************/

#include "buildTempTable.h"

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
