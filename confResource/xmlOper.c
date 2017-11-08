/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : xmlOper.c
* Descripe     : parse xml file
******************************************************/

#include "xmlOper.h"

static char error_info[LOGINFO_LENGTH];

bool ExtractFuncFromXML(char *xmlFilePath, char *tempFuncScoreTableName, char *tempFuncCallTableName)
{
    if(judgeCSrcXmlFile(xmlFilePath))
    {
        //handler C language xml File
        return ExtractFuncFromCXML(xmlFilePath, tempFuncScoreTableName, tempFuncCallTableName);
    }
    else
    {
        //handler C++ language xml File
        return ExtractFuncFromCPPXML(xmlFilePath, tempFuncScoreTableName, tempFuncCallTableName);
    }
}

funcCallInfoList *VarSclice(char *varName, char *xmlFilePath)
{
    if(judgeCSrcXmlFile(xmlFilePath))
    {
        //handler C language variable sclice
        return CSclice(varName, xmlFilePath);
    }
    else
    {
        //handler C++ language variable sclice
        return CPPSclice(varName, xmlFilePath);
    }
}

confVarDefValue getVarDefaultValue(char *varName, char *xmlFilePath)
{
    if(judgeCSrcXmlFile(xmlFilePath))
    {
        //handler C language variable sclice
        return getCVarDefaultValue(varName, xmlFilePath);
    }
    else
    {
        //handler C++ language variable sclice
        return getCPPVarDefaultValue(varName, xmlFilePath);
    }
}

bool JudgeVarInflFuncCallPath(char *varName, funcInfoList *funcCallPath)
{
    bool ret = false;
    funcInfoList *currentPath = funcCallPath;
    char tempVarName[64];
    memset(tempVarName, 0, 64);
    strcpy(tempVarName, varName);
    while(currentPath != NULL)
    {
        if(currentPath->next == NULL)
        {
           ret = true;
           break;
        }
        varDirectInflFuncList *begin = NULL;
        varDirectInflFuncList *current = NULL;
        char xmlFilePath[512];
        sprintf(xmlFilePath, "temp_%s.xml", currentPath->info.sourceFile);
        if(judgeCPreprocessFile(currentPath->info.sourceFile))
            begin = current = getCVarInfluFunc(tempVarName, currentPath->info.funcName, xmlFilePath, currentPath->info.argumentType);
        else
            begin = current = getCPPVarInfluFunc(tempVarName, currentPath->info.funcName, xmlFilePath, currentPath->info.argumentType);
        
        bool point = false;
        while(current != NULL)
        {
            if(strcasecmp(current->info.info.funcName, currentPath->next->info.funcName) == 0)
            {
                memset(xmlFilePath, 0, 512);
                sprintf(xmlFilePath, "temp_%s.xml", current->info.info.sourceFile);
                char *temp = getParaNameByIndex(current->info.index, current->info.info.funcName, xmlFilePath, \
                    current->info.info.argumentType);
                if(temp != NULL)
                {
                    memset(tempVarName, 0, 64);
                    strcpy(tempVarName, temp);
                    free(temp);
                    point = true;
                    break;
                }
                else
                {
                    memset(error_info, 0, LOGINFO_LENGTH);
                    sprintf(error_info, "%s: get function(%s) index = %d parameter failure!\n", current->info.info.sourceFile, \
                        current->info.info.funcName, current->info.index);
                    RecordLog(error_info);
                }
            }
                
            current = current->next;
        }
        current = begin;
        while(current != NULL)
        {
            begin = begin->next;
            free(current);
            current = begin;
        }
        if(!point)
        {
            ret = false;
            break;
        }
        
        currentPath = currentPath->next;
    }
    
    return ret;
}