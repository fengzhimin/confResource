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

bool JudgeVarInflFuncCallPath(char *varName, funcInfoList *funcCallPath, confScore *score)
{
    bool ret = false;
    funcInfoList *currentPath = funcCallPath;
    char tempVarName[64];
    memset(tempVarName, 0, 64);
    strcpy(tempVarName, varName);
    while(currentPath != NULL)
    {
        varDirectInflFuncList *begin = NULL;
        varDirectInflFuncList *current = NULL;
        char xmlFilePath[512];
        sprintf(xmlFilePath, "temp_%s.xml", currentPath->info.sourceFile);
        if(judgeCPreprocessFile(currentPath->info.sourceFile))
            begin = current = getCVarInfluFunc(tempVarName, currentPath->info.funcName, xmlFilePath, currentPath->info.argumentType);
        else
            begin = current = getCPPVarInfluFunc(tempVarName, currentPath->info.funcName, xmlFilePath, currentPath->info.argumentType);
        
        bool point = false;
        
        if(currentPath->next == NULL)
        {
            MYSQL temp_db;
            MYSQL *tempMysqlConnect = NULL;
            tempMysqlConnect = mysql_init(&temp_db);
            if(tempMysqlConnect == NULL)
            {
                RecordLog("init mysql failure\n");
                return false;
            }
            if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
                RecordLog(error_info);
                mysql_close(tempMysqlConnect);
                return false;
            }
            char temp_SqlCommand[LINE_CHAR_MAX_NUM] = "";
            sprintf(temp_SqlCommand, "select distinct argumentType from %s where funcName='%s' and funcCallType='%s' and sourceFile='%s'", \
                funcCallTableName, currentPath->info.funcName, currentPath->info.funcType, currentPath->info.sourceFile);
            if(mysql_real_query(tempMysqlConnect, temp_SqlCommand, strlen(temp_SqlCommand)) != 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
                RecordLog(error_info);
                mysql_close(tempMysqlConnect);
                return ret;
            }
            else
            {
                MYSQL_RES *res_ptr;
                res_ptr = mysql_store_result(tempMysqlConnect);
                int rownum = mysql_num_rows(res_ptr);
                //判断是否存在函数重载
                if(rownum <= 1)
                {
                    memset(temp_SqlCommand, 0, LINE_CHAR_MAX_NUM);
                    sprintf(temp_SqlCommand, "select calledFunc, line, forNum, whileNum from %s where funcName='%s' and funcCallType='%s' and sourceFile='%s' \
                    and type='L'", funcCallTableName, currentPath->info.funcName, currentPath->info.funcType, currentPath->info.sourceFile);
                }
                else
                {
                    int argumentNum = getSpecCharNumFromStr(currentPath->info.argumentType, '/') + 1;
                    char temp_argumentType[512] = "";
                    strcpy(temp_argumentType, currentPath->info.argumentType);
                    char selectArgumentType[512] = "(";
                    if(argumentNum == 1)
                    {
                        //only one argument or only void#0
                        if(strstr(temp_argumentType, "non") == NULL)
                            strcpy(selectArgumentType, temp_argumentType);
                        else
                            strcpy(selectArgumentType, "(%#1)");
                    }
                    else
                    {
                        //more than one argument
                        char (*arguType)[MAX_SUBSTR] = (char (*)[MAX_SUBSTR])malloc(argumentNum*MAX_SUBSTR);
                        int arguNum = 0;
                        removeChar(temp_argumentType, '(');
                        removeChar(temp_argumentType, ')');
                        removeChar(temp_argumentType, '#');
                        removeNum(temp_argumentType);
                        cutStrByLabel(temp_argumentType, '/', arguType, argumentNum);
                        
                        for(arguNum = 0; arguNum < argumentNum; arguNum++)
                        {
                            if(arguNum != 0)
                                strcat(selectArgumentType, "/");
                            if(strcasecmp(arguType[arguNum], "non") == 0)
                                strcat(selectArgumentType, "%");
                            else
                                strcat(selectArgumentType, arguType[arguNum]);
                        }
                        free(arguType);
                        sprintf(selectArgumentType, "%s#%d)", selectArgumentType, argumentNum);
                    }
                    memset(temp_SqlCommand, 0, LINE_CHAR_MAX_NUM);
                    sprintf(temp_SqlCommand, "select calledFunc, line, forNum, whileNum from %s where funcName='%s' and funcCallType='%s' and sourceFile='%s' \
                    and type='L' and argumentType like '%s'", funcCallTableName, currentPath->info.funcName, currentPath->info.funcType, \
                    currentPath->info.sourceFile, selectArgumentType);
                }
                mysql_free_result(res_ptr);
            }
                
            if(mysql_real_query(tempMysqlConnect, temp_SqlCommand, strlen(temp_SqlCommand)) != 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
                RecordLog(error_info);
                mysql_close(tempMysqlConnect);
                return false;
            }
            else
            {
                MYSQL_RES *res_ptr;
                res_ptr = mysql_store_result(tempMysqlConnect);
                int rownum = mysql_num_rows(res_ptr);
                //获取自定义函数调用库函数的记录
                for(int i = 0; i < rownum; i++)
                {
                    MYSQL_ROW sqlrow = mysql_fetch_row(res_ptr);
                    memset(temp_SqlCommand, 0, LINE_CHAR_MAX_NUM);
                    sprintf(temp_SqlCommand, "select type, score from funcLibrary where funcName='%s'", sqlrow[0]);
                    if(mysql_real_query(tempMysqlConnect, temp_SqlCommand, strlen(temp_SqlCommand)) != 0)
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
                        RecordLog(error_info);
                        mysql_close(tempMysqlConnect);
                        return false;
                    }
                    else
                    {
                        MYSQL_RES *temp_res_ptr;
                        temp_res_ptr = mysql_store_result(tempMysqlConnect);
                        int rownum = mysql_num_rows(temp_res_ptr);
                        if(rownum == 1)
                        {
                            MYSQL_ROW temp_sqlrow = mysql_fetch_row(temp_res_ptr);
                            
                            //判断调用库函数的自定义函数中是否配置项变量影响到了库函数
                            int temp_value = 0;
                            current = begin;
                            while(current != NULL)
                            {
                                if(strcasecmp(current->info.info.funcName, sqlrow[0]) == 0 && current->info.info.calledLine == StrToInt(sqlrow[1]))
                                {
                                    point = ret = true;
                                    temp_value = (currentConfOpt->defValue == 0 ? 1 : currentConfOpt->defValue)*StrToInt(temp_sqlrow[1]);
                                    
                                    break;
                                }
                                
                                current = current->next;
                            }
                            //判断库函数是否受到了循环的影响
                            if((StrToInt(sqlrow[2]) + StrToInt(sqlrow[3])) > 0)
                            {
                                loopExprList *loopInfo = getCalledFuncLoopInfo(currentPath->info.funcName, xmlFilePath, \
                                    currentPath->info.argumentType, sqlrow[0], StrToInt(sqlrow[1]));
                                
                                loopExprList *curLoopInfo = loopInfo;
                                while(curLoopInfo != NULL)
                                {
                                    if(strstr(curLoopInfo->expr.loopExpr, tempVarName) != NULL || strstr(curLoopInfo->expr.loopExpr, varName) != NULL)
                                    {
                                        if(temp_value)
                                            temp_value *= (currentConfOpt->defValue == 0 ? 1 : currentConfOpt->defValue);
                                        else
                                            temp_value = (currentConfOpt->defValue == 0 ? 1 : currentConfOpt->defValue);
                                    }
                                    else
                                    {
                                        //判断influedVarName是否被varName影响
                                        varDef *beginInfluVar = NULL;
                                        varDef *currentInfluVar = NULL;
                                        beginInfluVar = currentInfluVar = getVarInfluVarInfo(tempVarName, currentPath->info.funcName, xmlFilePath, currentPath->info.argumentType);
                                        while(currentInfluVar != NULL)
                                        {
                                            if(strstr(curLoopInfo->expr.loopExpr, currentInfluVar->varName) != NULL)
                                            {
                                                if(temp_value)
                                                    temp_value *= (currentConfOpt->defValue == 0 ? 1 : currentConfOpt->defValue);
                                                else
                                                    temp_value = (currentConfOpt->defValue == 0 ? 1 : currentConfOpt->defValue);
                                                break;
                                            }
                                            currentInfluVar = currentInfluVar->next;
                                        }
                                        currentInfluVar = beginInfluVar;
                                        while(currentInfluVar != NULL)
                                        {
                                            beginInfluVar = beginInfluVar->next;
                                            free(currentInfluVar);
                                            currentInfluVar = beginInfluVar;
                                        }
                                    }
                                        
                                    curLoopInfo = curLoopInfo->next;
                                }
                                curLoopInfo = loopInfo;
                                while(curLoopInfo != NULL)
                                {
                                    loopInfo = loopInfo->next;
                                    free(curLoopInfo);
                                    curLoopInfo = loopInfo;
                                }
                            }
                            if(strcasecmp(temp_sqlrow[0], "CPU") == 0)
                            {
                                score->CPU += (temp_value - StrToInt(temp_sqlrow[1]));
                            }
                            else if(strcasecmp(temp_sqlrow[0], "MEM") == 0)
                            {
                                score->MEM += (temp_value - StrToInt(temp_sqlrow[1]));
                            }
                            else if(strcasecmp(temp_sqlrow[0], "IO") == 0)
                            {
                                score->IO += (temp_value - StrToInt(temp_sqlrow[1]));
                            }
                            else if(strcasecmp(temp_sqlrow[0], "NET") == 0)
                            {
                                score->NET += (temp_value - StrToInt(temp_sqlrow[1]));
                            }
                        }
                        mysql_free_result(temp_res_ptr);
                    }
                }
                mysql_free_result(res_ptr);
            }
            
            mysql_close(tempMysqlConnect);
        }
        else
        {
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

bool JudgeVarInflSpeciVarByFuncCallPath(char *varName, char *influedVarName, funcInfoList *funcCallPath)
{
    bool ret = false;
    funcInfoList *currentPath = funcCallPath;
    char tempVarName[64];
    memset(tempVarName, 0, 64);
    strcpy(tempVarName, varName);
    while(currentPath != NULL)
    {
        char xmlFilePath[512];
        sprintf(xmlFilePath, "temp_%s.xml", currentPath->info.sourceFile);
        if(currentPath->next == NULL)
        {
            //判断存在指定分析变量的自定义函数中是否被配置项变量影响到
            //首先判断函数参数是否为要被影响变量的
            if(strcasecmp(varName, influedVarName) == 0)
            {
                ret = true;
                break;
            }
            //判断influedVarName是否被varName影响
            varDef *beginInfluVar = NULL;
            varDef *currentInfluVar = NULL;
            beginInfluVar = currentInfluVar = getVarInfluVarInfo(tempVarName, currentPath->info.funcName, xmlFilePath, currentPath->info.argumentType);
            while(currentInfluVar != NULL)
            {
                if(strcasecmp(currentInfluVar->varName, influedVarName) == 0)
                {
                    ret = true;
                    break;
                }
                currentInfluVar = currentInfluVar->next;
            }
            currentInfluVar = beginInfluVar;
            while(currentInfluVar != NULL)
            {
                beginInfluVar = beginInfluVar->next;
                free(currentInfluVar);
                currentInfluVar = beginInfluVar;
            }
            
            break;
        }
        varDirectInflFuncList *begin = NULL;
        varDirectInflFuncList *current = NULL;
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

varDef *ExtractVarInflVarByFuncCallPath(char *varName, funcInfoList *funcCallPath)
{
    funcInfoList *currentPath = funcCallPath;
    char tempVarName[64];
    memset(tempVarName, 0, 64);
    strcpy(tempVarName, varName);
    while(currentPath != NULL)
    {
        char xmlFilePath[512];
        sprintf(xmlFilePath, "temp_%s.xml", currentPath->info.sourceFile);
        if(currentPath->next == NULL)
        {
            //直接获取最后一个函数中被影响的变量信息
            //将被影响的函数参数变量作为第一个被影响的变量
            varDef *begin = NULL;
            varDef *end = NULL;
            begin = end = malloc(sizeof(varDef));
            memset(begin, 0, sizeof(varDef));
            strcpy(end->varName, tempVarName);
            end->next = getVarInfluVarInfo(tempVarName, currentPath->info.funcName, xmlFilePath, currentPath->info.argumentType);
            
            return begin;
        }
        varDirectInflFuncList *begin = NULL;
        varDirectInflFuncList *current = NULL;
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
            break;
        }
        
        currentPath = currentPath->next;
    }
    
    return NULL;
}