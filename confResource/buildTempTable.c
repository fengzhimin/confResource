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
    
    //create function call table
    if(executeCommand(createFuncCallTable))
        ret = true;
    else
    {
        RecordLog("create funcCall table failure!\n");
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
    
    //create function call table
    if(executeCommand(deleteFuncCallTable))
        ret = true;
    else
    {
        RecordLog("delete funcCall table failure!\n");
        ret = false;
    }
    
    return ret;
}
