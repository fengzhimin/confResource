/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CXmlOper.c
* Descripe     : parse C language xml file
******************************************************/

#include "CXmlOper.h"
#include "sourceOper.h"

static char error_info[LOGINFO_LENGTH];

#define scanCallFunction(tempFuncCallTableName, cur, funcName, funcType, funcArgumentType, srcPath, varTypeBegin)   \
    scanCallFunctionFromNode(tempFuncCallTableName, cur, funcName, funcType, funcArgumentType, srcPath, varTypeBegin, true)

static void scanCallFunctionFromNode(char *tempFuncCallTableName, xmlNodePtr cur, char *funcName, char *funcType, char *funcArgumentType, \
    char *srcPath, varType *varTypeBegin, bool flag);

bool ExtractFuncFromCXML(char *xmlFilePath, char *tempFuncScoreTableName, char *tempFuncCallTableName)
{
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		RecordLog(error_info);
        return false;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return false;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            xmlNodePtr temp_cur = funcNode->children;
            char *funcType = "extern";
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"specifier"))
                {
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), "static") == 0)
                        funcType = "static";
                }
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(strcasecmp("__attribute__", (char*)xmlNodeGetContent(temp_cur)) == 0)
                        break;
                    char src_dir[DIRPATH_MAX] = "";
                    //删除开头的temp_和结尾的.xml
                    strncpy(src_dir, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                    xmlChar* attr_value = getLine(temp_cur);
                    char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
                    //get function argument type string
                    char *argumentTypeString = ExtractFuncArgumentType(funcNode);
                    sprintf(tempSqlCommand, "insert into %s (funcName, type, argumentType, sourceFile, line) value('%s', '%s', '%s', '%s', %s)", tempFuncScoreTableName,\
                        (char*)xmlNodeGetContent(temp_cur), funcType, argumentTypeString, src_dir, attr_value);
                    if(!executeSQLCommand(NULL, tempSqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
                        RecordLog(error_info);
                    }
                    varType *begin = ExtractVarType(funcNode);
                    varType *current = begin;
                    scanCallFunction(tempFuncCallTableName, funcNode, (char*)xmlNodeGetContent(temp_cur), funcType, argumentTypeString, src_dir, begin);
                    while(current != NULL)
                    {
                        begin = begin->next;
                        //printf("%s(%d):%s\n", current->type, current->line, current->varName);
                        free(current);
                        current = begin;
                    }
                    free(argumentTypeString);
                    break;
                }
                temp_cur = temp_cur->next;
            }
            
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    optDataBaseOper(tempFuncScoreTableName, tempFuncCallTableName);
    return true;  
}

/*******************************
 * func: self-define function scan call function
 * return: void
 * @para cur: current self-define function Node
 * @para funcName: current self-define function name
 * @para funcType: current self-define function type(extern or static)
 * @para srcPath: function source file path
 * @para varTypeBegin: variable type header point
********************************/
static void scanCallFunctionFromNode(char *tempFuncCallTableName, xmlNodePtr cur, char *funcName, char *funcType, char *funcArgumentType, char *srcPath, varType *varTypeBegin, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                xmlChar* attr_value = getLine(cur->children);
                char *callFuncName = NULL;
                if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                {
                    callFuncName = (char*)xmlNodeGetContent(cur->children->last);
                }
                else
                {
                    callFuncName = (char*)xmlNodeGetContent(cur->children);
                }
                //删除递归调用
                if(strcasecmp(callFuncName, funcName) != 0 && strcasecmp(callFuncName, "__attribute__") != 0)
                {
                    xmlNodePtr parentNode = cur->parent;
                    int forNum = 0;
                    int whileNum = 0;
                    while(parentNode != NULL)
                    {
                        if(!xmlStrcmp(parentNode->name, (const xmlChar*)"for"))
                            forNum++;
                        else if(!xmlStrcmp(parentNode->name, (const xmlChar*)"do") || !xmlStrcmp(parentNode->name, (const xmlChar*)"while"))
                            whileNum++;
                        parentNode = parentNode->parent;
                    }
                    char *calledFuncArgumentTypeString = getCalledFuncArgumentType(cur, varTypeBegin);
                    char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
                    sprintf(tempSqlCommand, "insert into %s (funcName, funcCallType, argumentType, sourceFile, calledFunc, calledFuncArgumentType, CalledSrcFile, line, type, forNum, whileNum) \
                        value('%s', '%s', '%s', '%s', '%s', '%s', '%s', %s, 'L', %d, %d)", tempFuncCallTableName, funcName, funcType, funcArgumentType, srcPath, callFuncName, \
                        calledFuncArgumentTypeString, srcPath, attr_value, forNum, whileNum);
                    free(calledFuncArgumentTypeString);
                    if(!executeSQLCommand(NULL, tempSqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
                        RecordLog(error_info);
                    }
                }
            }
        }
        
        scanCallFunctionFromNode(tempFuncCallTableName, cur->children, funcName, funcType, funcArgumentType, srcPath, varTypeBegin, false);
        if(flag)
            break;
        cur = cur->next;
    }
}

funcInfoList *scanCCallFuncFromNode(xmlNodePtr cur, varType *varTypeBegin, bool flag)
{
    funcInfoList *begin = NULL;
    funcInfoList *end = NULL;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                xmlChar* attr_value =getLine(cur->children);
                char *calledFuncName = NULL;
                if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                {
                    calledFuncName = (char*)xmlNodeGetContent(cur->children->last);
                }
                else
                {
                    calledFuncName = (char*)xmlNodeGetContent(cur->children);
                }
                
                if(begin == NULL)
                {
                    begin = end = malloc(sizeof(funcInfoList));
                    memset(end, 0, sizeof(funcInfoList));
                }
                else
                {
                    end->next = malloc(sizeof(funcInfoList));
                    memset(end->next, 0, sizeof(funcInfoList));
                    end->next->prev = end;
                    end = end->next;
                }

                strcpy(end->info.funcName, calledFuncName);
                end->info.calledLine = StrToInt((char *)attr_value);
                //get called function argument type and filePath
                char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
                int rownum = 0;
                sprintf(tempSqlCommand, "select calledFuncType, calledFuncArgumentType, CalledSrcFile from %s where calledFunc='%s' and line=%s",\
                    funcCallTableName, calledFuncName, (char *)attr_value);
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
                if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
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
                    MYSQL_ROW sqlrow;
                    res_ptr = mysql_store_result(tempMysqlConnect);
                    rownum = mysql_num_rows(res_ptr);
                    //count 为递归的最大深度
                    if(rownum != 0)
                    {
                        //self-defined function
                        while((sqlrow = mysql_fetch_row(res_ptr)) != NULL)
                        {
                            strcpy(end->info.funcType, sqlrow[0]);
                            strcpy(end->info.argumentType, sqlrow[1]);
                            strcpy(end->info.sourceFile, sqlrow[2]);
                            //sprintf(end->info.sourceFile, "temp_%s.xml", sqlrow[2]);
                            end->info.type = 'S';
                            mysql_free_result(res_ptr);
                            break;
                        }
#if DEBUG == 1         
                        printf("%s(%s)\n", calledFuncName, attr_value);
#endif
                    }
                    else
                    {
                        //library function
                        end->info.type = 'L';
#if DEBUG == 1         
                        printf("%s(%s)\n", calledFuncName, attr_value);
#endif
                    }
                }
                mysql_close(tempMysqlConnect);
            }
        }
        
        funcInfoList *current = scanCCallFuncFromNode(cur->children, varTypeBegin, false);
        if(current != NULL)
        {
            if(begin == NULL)
            {
                begin = end = current;
            }
            else
            {
                end->next = current;
                current->prev = end;
            }
        }
        //update end value
        while(current != NULL)
        {
            end = current;
            current = current->next;
        }
        if(flag)
            break;
        cur = cur->next;
    }
    
    return begin;
}

funcInfoList *scanBackCCallFunc(xmlNodePtr cur, varType *varTypeBegin)
{
    funcInfoList *begin = NULL;
    funcInfoList *end = NULL;
    funcInfoList *current = NULL;
    if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
    {
        return NULL;
    }
    xmlNodePtr temp_cur = cur;
    cur = cur->next;
    while(cur != NULL)
    {
        current = scanCCallFunc(cur, varTypeBegin);
        if(current != NULL)
        {
            if(begin == NULL)
            {
                begin = end = current;
            }
            else
            {
                end->next = current;
                current->prev = end;
            }
        }
        //update end value
        while(current != NULL)
        {
            end = current;
            current = current->next;
        }
        cur = cur->next;
    }
    
    current = scanBackCCallFunc(temp_cur->parent, varTypeBegin);
    if(current != NULL)
    {
        if(begin == NULL)
        {
            begin = end = current;
        }
        else
        {
            end->next = current;
            current->prev = end;
        }
    }
    //update end value
    while(current != NULL)
    {
        end = current;
        current = current->next;
    }
    
    return begin;
}

funcInfoList *varCScliceFuncFromNode(varDef varInfo, xmlNodePtr cur, varType *varTypeBegin, bool flag)
{
    funcInfoList *begin = NULL;
    funcInfoList *end = NULL;
    funcInfoList *current = NULL;
    int currentLine = 0;
    while(cur != NULL)
    {
        bool recursive_flag = true;
        if(!xmlStrcmp(cur->name, (const xmlChar*)"if"))
        {
            currentLine = StrToInt((char *)getLine(cur));
            if(varInfo.line < currentLine)
            {
                xmlNodePtr condition = cur->children;
                while(condition != NULL)
                {
                    if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                    {
                        //判断if条件中是否使用了varName变量，如果使用则该变量影响整个if块
                        if(JudgeVarUsed(condition, varInfo.varName))
                        {
                            //打印整个if-else结构块                   
                            current = scanCCallFunc(cur, varTypeBegin);
                            if(begin == NULL)
                            {
                                begin = end = current;
                            }
                            else if(current != NULL)
                            {
                                end->next = current;
                                current->prev = end;
                                end = end->next;
                            }                    
                            bool isExit = false;
                            while(current != NULL)
                            {
                                if(strcasecmp(current->info.funcName, "exit") == 0)
                                    isExit = true;
                                end = current;
                                current = current->next;
                            }
                            //scanAssignVar(cur);
                            if(isExit)
                            {
                                current = scanBackCCallFunc(cur, varTypeBegin);
                                if(begin == NULL)
                                {
                                    begin = end = current;
                                }
                                else if(current != NULL)
                                {
                                    end->next = current;
                                    current->prev = end;
                                    end = end->next;
                                } 
                                while(current != NULL)
                                {
                                    end = current;
                                    current = current->next;
                                }                       
                            }
                            else
                            {
                                xmlNodePtr then = condition->next;
                                while(then != NULL)
                                {
                                    if(JudgeExistChildNode(then, "return"))
                                    {                              
                                        current = scanBackCCallFunc(cur, varTypeBegin);
                                        if(begin == NULL)
                                        {
                                            begin = end = current;
                                        }
                                        else if(current != NULL)
                                        {
                                            end->next = current;
                                            current->prev = end;
                                            end = end->next;
                                        } 
                                        while(current != NULL)
                                        {
                                            end = current;
                                            current = current->next;
                                        }                       
                                        //scanBackAssignVar(cur);
                                        break;
                                    }
                                    then = then->next;
                                }
                            }
                            
                            recursive_flag = false;
                            break;
                        }
                    }
                    condition = condition->next;
                }
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"while"))
        {
            currentLine = StrToInt((char *)getLine(cur));
            if(varInfo.line < currentLine)
            {
                xmlNodePtr condition = cur->children;
                while(condition != NULL)
                {
                    if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                    {
                        //判断while条件中是否使用了varName变量，如果使用则该变量影响整个while块
                        if(JudgeVarUsed(condition, varInfo.varName))
                        {
                            //打印整个while结构块                       
                            current = scanCCallFunc(cur, varTypeBegin);                       
                            if(begin == NULL)
                            {
                                begin = end = current;
                            }
                            else if(current != NULL)
                            {
                                end->next = current;
                                current->prev = end;
                                end = end->next;
                            } 
                            bool isExit = false;
                            while(current != NULL)
                            {
                                if(strcasecmp(current->info.funcName, "exit") == 0)
                                    isExit = true;
                                end = current;
                                current = current->next;
                            }
                            //scanAssignVar(cur);
                            if(isExit)
                            {
                                current = scanBackCCallFunc(cur, varTypeBegin);
                                if(begin == NULL)
                                {
                                    begin = end = current;
                                }
                                else if(current != NULL)
                                {
                                    end->next = current;
                                    current->prev = end;
                                    end = end->next;
                                } 
                                while(current != NULL)
                                {
                                    end = current;
                                    current = current->next;
                                }                       
                            }                      
                            else
                            {
                                xmlNodePtr block = condition->next;
                                while(block != NULL)
                                {
                                    if(JudgeExistChildNode(block, "return"))
                                    {                               
                                        current = scanBackCCallFunc(cur, varTypeBegin);
                                        if(begin == NULL)
                                        {
                                            begin = end = current;
                                        }
                                        else if(current != NULL)
                                        {
                                            end->next = current;
                                            current->prev = end;
                                            end = end->next;
                                        } 
                                        while(current != NULL)
                                        {
                                            end = current;
                                            current = current->next;
                                        }                                
                                        //scanBackAssignVar(cur);
                                        break;
                                    }
                                    block = block->next;
                                }
                            }
                            
                            recursive_flag = false;
                            break;
                        }
                    }
                    condition = condition->next;
                }
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"do"))
        {
            currentLine = StrToInt((char *)getLine(cur));
            if(varInfo.line < currentLine)
            {
                xmlNodePtr condition = cur->children;
                while(condition != NULL)
                {
                    if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                    {
                        //判断do-while条件中是否使用了varName变量，如果使用则该变量影响整个do-while块
                        if(JudgeVarUsed(condition, varInfo.varName))
                        {
                            //打印整个do-while结构块
                            current = scanCCallFunc(cur, varTypeBegin);                       
                            if(begin == NULL)
                            {
                                begin = end = current;
                            }
                            else if(current != NULL)
                            {
                                end->next = current;
                                current->prev = end;
                                end = end->next;
                            } 
                            bool isExit = false;
                            while(current != NULL)
                            {
                                if(strcasecmp(current->info.funcName, "exit") == 0)
                                    isExit = true;
                                end = current;
                                current = current->next;
                            }
                            //scanAssignVar(cur);
                            if(isExit)
                            {
                                current = scanBackCCallFunc(cur, varTypeBegin);
                                if(begin == NULL)
                                {
                                    begin = end = current;
                                }
                                else if(current != NULL)
                                {
                                    end->next = current;
                                    current->prev = end;
                                    end = end->next;
                                } 
                                while(current != NULL)
                                {
                                    end = current;
                                    current = current->next;
                                }                       
                            }
                            else
                            {
                                xmlNodePtr block = condition->prev;
                                while(block != NULL)
                                {
                                    if(JudgeExistChildNode(block, "return"))
                                    {
                                        current = scanBackCCallFunc(cur, varTypeBegin);
                                        if(begin == NULL)
                                        {
                                            begin = end = current;
                                        }
                                        else if(current != NULL)
                                        {
                                            end->next = current;
                                            current->prev = end;
                                            end = end->next;
                                        } 
                                        while(current != NULL)
                                        {
                                            end = current;
                                            current = current->next;
                                        }
                                        //scanBackAssignVar(cur);
                                        break;
                                    }
                                    block = block->prev;
                                }
                            }
                            
                            recursive_flag = false;
                            break;
                        }
                    }
                    condition = condition->next;
                }
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"for"))
        {
            currentLine = StrToInt((char *)getLine(cur));
            if(varInfo.line < currentLine)
            {
                xmlNodePtr control = cur->children;
                while(control != NULL)
                {
                    if(!xmlStrcmp(control->name, (const xmlChar*)"control"))
                    {
                        xmlNodePtr condition = control->children;
                        while(condition != NULL)
                        {
                            //判断for条件中是否使用了varName变量，如果使用则该变量影响整个for块
                            if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                            {
                                if(JudgeVarUsed(condition, varInfo.varName))
                                {
                                    //打印整个for结构块
                                    current = scanCCallFunc(cur, varTypeBegin);                       
                                    if(begin == NULL)
                                    {
                                        begin = end = current;
                                    }
                                    else if(current != NULL)
                                    {
                                        end->next = current;
                                        current->prev = end;
                                        end = end->next;
                                    } 
                                    bool isExit = false;
                                    while(current != NULL)
                                    {
                                        if(strcasecmp(current->info.funcName, "exit") == 0)
                                            isExit = true;
                                        end = current;
                                        current = current->next;
                                    }
                                    //scanAssignVar(cur);
                                    if(isExit)
                                    {
                                        current = scanBackCCallFunc(cur, varTypeBegin);
                                        if(begin == NULL)
                                        {
                                            begin = end = current;
                                        }
                                        else if(current != NULL)
                                        {
                                            end->next = current;
                                            current->prev = end;
                                            end = end->next;
                                        } 
                                        while(current != NULL)
                                        {
                                            end = current;
                                            current = current->next;
                                        }                       
                                    }
                                    else
                                    {
                                        xmlNodePtr block = control->next;
                                        while(block != NULL)
                                        {
                                            if(JudgeExistChildNode(block, "return"))
                                            {
                                                current = scanBackCCallFunc(cur, varTypeBegin);
                                                if(begin == NULL)
                                                {
                                                    begin = end = current;
                                                }
                                                else if(current != NULL)
                                                {
                                                    end->next = current;
                                                    current->prev = end;
                                                    end = end->next;
                                                } 
                                                while(current != NULL)
                                                {
                                                    end = current;
                                                    current = current->next;
                                                }
                                                //scanBackAssignVar(cur);
                                                break;
                                            }
                                            block = block->next;
                                        }
                                    }
                                    
                                    recursive_flag = false;
                                    break;
                                }
                            }
                            condition = condition->next;
                        }
                        break;
                    }
                    control = control->next;
                }
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            recursive_flag = false;
            xmlNodePtr argument_list = cur->children;
            xmlChar* attr_value = NULL;
            char *calledFuncName = NULL;
            while(argument_list != NULL)
            {
                if(!xmlStrcmp(argument_list->name, (const xmlChar*)"name"))
                {
                    attr_value = getLine(argument_list);
                    calledFuncName = (char*)xmlNodeGetContent(argument_list);
                    currentLine = StrToInt((char *)attr_value);
                    if(varInfo.line > currentLine)
                        break;
                }
                else if(!xmlStrcmp(argument_list->name, (const xmlChar*)"argument_list"))
                {
                    if(JudgeVarUsed(argument_list, varInfo.varName))
                    {
                        if(begin == NULL)
                        {
                            begin = end = malloc(sizeof(funcInfoList));
                            memset(end, 0, sizeof(funcInfoList));
                        }
                        else
                        {
                            end->next = malloc(sizeof(funcInfoList));
                            memset(end->next, 0, sizeof(funcInfoList));
                            end->next->prev = end;
                            end = end->next;
                        }
                        
                        strcpy(end->info.funcName, calledFuncName);
                        end->info.calledLine = StrToInt((char *)attr_value);
                        //get called function argument type and filePath
                        char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
                        sprintf(tempSqlCommand, "select calledFuncType, calledFuncArgumentType, CalledSrcFile from %s where calledFunc='%s' and line=%s",\
                            funcCallTableName, calledFuncName, (char *)attr_value);
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
                        if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
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
                            MYSQL_ROW sqlrow;
                            res_ptr = mysql_store_result(tempMysqlConnect);
                            int rownum = mysql_num_rows(res_ptr);

                            if(rownum != 0)
                            {
                                while((sqlrow = mysql_fetch_row(res_ptr)) != NULL)
                                {
                                    strcpy(end->info.funcType, sqlrow[0]);
                                    strcpy(end->info.argumentType, sqlrow[1]);
                                    strcpy(end->info.sourceFile, sqlrow[2]);
                                    //sprintf(end->info.sourceFile, "temp_%s.xml", sqlrow[2]);
                                    end->info.type = 'S';
                                    mysql_free_result(res_ptr);
                                    break;
                                }
#if DEBUG == 1
                                printf("%s(%s)\n", calledFuncName, attr_value);
#endif
                            }
                            else
                            {
                                //library function
                                end->info.type = 'L';
#if DEBUG == 1
                                printf("%s(%s)\n", calledFuncName, attr_value);
#endif
                            }
                        }
                        mysql_close(tempMysqlConnect);
                    }
                    
                    //handle function call as argument
                    xmlNodePtr argument = argument_list->children;
                    while(argument != NULL)
                    {
                        if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
                        {
                            current = varCScliceFuncFromNode(varInfo, argument->children, varTypeBegin, false);
                            if(begin == NULL)
                            {
                                begin = end = current;
                            }
                            else if(current != NULL)
                            {
                                end->next = current;
                                current->prev = end;
                                end = end->next;
                            }
                            while(current != NULL)
                            {
                                end = current;
                                current = current->next;
                            }
                        }
                        argument = argument->next;
                    }
                }
                argument_list = argument_list->next;
            }
        }
        
        if(recursive_flag)
        {
            current = varCScliceFuncFromNode(varInfo, cur->children, varTypeBegin, false);
            if(begin == NULL)
            {
                begin = end = current;
            }
            else if(current != NULL)
            {
                end->next = current;
                current->prev = end;
                end = end->next;
            }
            while(current != NULL)
            {
                end = current;
                current = current->next;
            }
        }
        
        if(flag)
            break;
        cur = cur->next;
    }
    
    return begin;
}

funcCallInfoList *CSclice(char *varName, char *xmlFilePath)
{
#if DEBUG == 1
    return ScliceDebug(varName, xmlFilePath, varCScliceFuncFromNode);
#else
    return Sclice(varName, xmlFilePath, varCScliceFuncFromNode);
#endif
}

varDirectInflFuncList *getCDirectInflFuncFromNode(char *varName, xmlNodePtr funcBlockNode, varType *varTypeBegin, bool flag)
{
    varDirectInflFuncList *begin = NULL;
    varDirectInflFuncList *end = NULL;
    varDirectInflFuncList *current = NULL;
    while(funcBlockNode != NULL)
    {
        if(!xmlStrcmp(funcBlockNode->name, (const xmlChar*)"call"))
        {
            xmlNodePtr argument_list = funcBlockNode->children;
            xmlChar* attr_value = NULL;
            char *calledFuncName = NULL;
            while(argument_list != NULL)
            {
                if(!xmlStrcmp(argument_list->name, (const xmlChar*)"name"))
                {
                    attr_value = getLine(argument_list);
                    calledFuncName = (char*)xmlNodeGetContent(argument_list);
                }
                else if(!xmlStrcmp(argument_list->name, (const xmlChar*)"argument_list"))
                {
                    int paraPosition = getArguPosition(varName, argument_list);
                    if(paraPosition != -1)
                    {
                        //get called function filePath
                        char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
                        sprintf(tempSqlCommand, "select calledFuncType, calledFuncArgumentType, CalledSrcFile, type from %s where calledFunc='%s' and line=%s",\
                            funcCallTableName, calledFuncName, (char *)attr_value);
                        MYSQL temp_db;
                        MYSQL *tempMysqlConnect = NULL;
                        tempMysqlConnect = mysql_init(&temp_db);
                        if(tempMysqlConnect == NULL)
                        {
                            RecordLog("init mysql failure\n");
                            return NULL;
                        }
                        if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
                        {
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
                            RecordLog(error_info);
                            mysql_close(tempMysqlConnect);
                            return NULL;
                        }
                        if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
                        {
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
                            RecordLog(error_info);
                            mysql_close(tempMysqlConnect);
                            return NULL;
                        }
                        else
                        {
                            MYSQL_RES *res_ptr;
                            MYSQL_ROW sqlrow;
                            res_ptr = mysql_store_result(tempMysqlConnect);
                            int rownum = mysql_num_rows(res_ptr);

                            if(rownum != 0)
                            {
                                while((sqlrow = mysql_fetch_row(res_ptr)) != NULL)
                                {
                                    if(begin == NULL)
                                        begin = end = malloc(sizeof(varDirectInflFuncList));
                                    else
                                        end = end->next = malloc(sizeof(varDirectInflFuncList));
                                    memset(end, 0, sizeof(varDirectInflFuncList));
                                    strcpy(end->info.info.funcName, calledFuncName);
                                    end->info.info.calledLine = StrToInt((char *)attr_value);
                                    strcpy(end->info.info.funcType, sqlrow[0]);
                                    strcpy(end->info.info.argumentType, sqlrow[1]);
                                    strcpy(end->info.info.sourceFile, sqlrow[2]);
                                    //sprintf(end->info.info.sourceFile, "temp_%s.xml", sqlrow[2]);
                                    end->info.info.type = ((char *)sqlrow[3])[0];
                                    end->info.index = paraPosition;
                                    mysql_free_result(res_ptr);
                                    break;
                                }
                            }
                        }
                        mysql_close(tempMysqlConnect);
                    }
                }
                argument_list = argument_list->next;
            }
        }
        current = getCDirectInflFuncFromNode(varName, funcBlockNode->children, varTypeBegin, false);
        if(begin == NULL)
            begin = end = current;
        else if(current != NULL)
            end = end->next = current;
        while(current != NULL)
        {
            end = current;
            current = current->next;
        }
        if(flag)
            break;
        funcBlockNode = funcBlockNode->next;
    }
    
    return begin;
}

confVarDefValue getCVarDefaultValue(char *varName, char *xmlFilePath)
{
    confVarDefValue ret;
    ret.defValue = -1;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		RecordLog(error_info);
        return ret;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return ret;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
                
            if(JudgeVarUsed(funcNode, varName))
            {
                ret = getVarDefValue(varName, funcNode);
                if(ret.defValue == -1)
                {
                    varDirectInflFuncList *begin = NULL;
                    varDirectInflFuncList *current = NULL;

                    begin = current = getCDirectInflFunc(varName, funcNode);
                    while(current != NULL)
                    {
                        char xmlFilePath[512];
                        sprintf(xmlFilePath, "temp_%s.xml", current->info.info.sourceFile);
                        if(judgeCPreprocessFile(current->info.info.sourceFile))
                            ret = ExtractSpeciParaDefValue(current->info.index, current->info.info.funcName, xmlFilePath, \
                                current->info.info.argumentType, getCDirectInflFuncFromNode);
                        else
                            ret = ExtractSpeciParaDefValue(current->info.index, current->info.info.funcName, xmlFilePath, \
                                current->info.info.argumentType, getCPPDirectInflFuncFromNode);
                        if(ret.defValue != -1)
                            break;
                        current = current->next;
                    }
                    current = begin;
                    while(current != NULL)
                    {
                        begin = begin->next;
                        free(current);
                        current = begin;
                    }
                }
                xmlFreeDoc(doc);
                return ret;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    
    return ret;
}

varDirectInflFuncList *getCVarInfluFunc(char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType)
{
    return getVarInfluFunc(varName, funcName, xmlFilePath, funcArgumentType, getCDirectInflFuncFromNode);
}

bool ExtractConfKeyUsedInfoFromNode(xmlNodePtr cur, char *confName, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"literal"))
        {
            char *content = (char *)xmlNodeGetContent(cur);
            char *type = (char *)xmlGetProp(cur, (xmlChar*)"type");
            if(strcasecmp(type, "string") == 0)
            {
                char *temp_content = ExtractStringFromLiteral(content);
                if(strcasecmp(confName, temp_content) == 0)
                {
                    free(temp_content);
                    return true;
                }
                free(temp_content);
            }
        }
        else if(ExtractConfKeyUsedInfoFromNode(cur->children, confName, false))
            return true;
        
        if(flag)
            break;
        cur = cur->next;
    }
    
    return  false;
}

bool literalScliceVarFromNode(char *literalName, xmlNodePtr cur, bool flag)
{
    bool ret = false;
    
    while(cur != NULL)
    {
        bool recursive_flag = true;
        if(!xmlStrcmp(cur->name, (const xmlChar*)"if"))
        {
            xmlNodePtr condition = cur->children;
            while(condition != NULL)
            {
                if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                {
                    //判断if条件中是否使用了literalName常量，如果使用则该变量影响整个if块
                    if(ExtractConfKeyUsedInfo(condition, literalName))
                    {
                        //打印整个if-else结构块
                        scanAssignVar(cur);
                        xmlNodePtr then = condition->next;
                        while(then != NULL)
                        {
                            if(JudgeExistChildNode(then, "return"))
                            {
                                scanBackAssignVar(cur);
                                break;
                            }
                            then = then->next;
                        }
                        recursive_flag = false;
                        ret = true;
                        break;
                    }
                }
                condition = condition->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"while"))
        {
            xmlNodePtr condition = cur->children;
            while(condition != NULL)
            {
                if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                {
                    //判断while条件中是否使用了literalName常量，如果使用则该变量影响整个while块
                    if(ExtractConfKeyUsedInfo(condition, literalName))
                    {
                        //打印整个while结构块
                        scanAssignVar(cur);
                        xmlNodePtr block = condition->next;
                        while(block != NULL)
                        {
                            if(JudgeExistChildNode(block, "return"))
                            {
                                scanBackAssignVar(cur);
                                break;
                            }
                            block = block->next;
                        }
                        recursive_flag = false;
                        ret = true;
                        break;
                    }
                }
                condition = condition->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"do"))
        {
            xmlNodePtr condition = cur->children;
            while(condition != NULL)
            {
                if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                {
                    //判断do-while条件中是否使用了literalName常量，如果使用则该变量影响整个do-while块
                    if(ExtractConfKeyUsedInfo(condition, literalName))
                    {
                        //打印整个do-while结构块
                        scanAssignVar(cur);
                        xmlNodePtr block = condition->prev;
                        while(block != NULL)
                        {
                            if(JudgeExistChildNode(block, "return"))
                            {
                                scanBackAssignVar(cur);
                                break;
                            }
                            block = block->prev;
                        }
                        recursive_flag = false;
                        ret = true;
                        break;
                    }
                }
                condition = condition->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"for"))
        {
            xmlNodePtr control = cur->children;
            while(control != NULL)
            {
                if(!xmlStrcmp(control->name, (const xmlChar*)"control"))
                {
                    xmlNodePtr condition = control->children;
                    while(condition != NULL)
                    {
                        if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                        {
                            //判断for条件中是否使用了literalName变量，如果使用则该变量影响整个for块
                            if(ExtractConfKeyUsedInfo(condition, literalName))
                            {
                                //打印整个for结构块
                                scanAssignVar(cur);
                                xmlNodePtr block = control->next;
                                while(block != NULL)
                                {
                                    if(JudgeExistChildNode(block, "return"))
                                    {
                                        scanBackAssignVar(cur);
                                        break;
                                    }
                                    block = block->next;
                                }
                                recursive_flag = false;
                                ret = true;
                                break;
                            }
                        }
                        condition = condition->next;
                    }
                    break;
                }
                control = control->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            recursive_flag = false;
            xmlNodePtr argument_list = cur->children;
            xmlChar* attr_value = NULL;
            char *calledFuncName = NULL;
            while(argument_list != NULL)
            {
                if(!xmlStrcmp(argument_list->name, (const xmlChar*)"name"))
                {
                    attr_value = getLine(argument_list);
                    calledFuncName = (char*)xmlNodeGetContent(argument_list);
                }
                else if(!xmlStrcmp(argument_list->name, (const xmlChar*)"argument_list"))
                {
                    xmlNodePtr argument = argument_list->children;
                    if(ExtractConfKeyUsedInfo(argument_list, literalName))
                    {
                        printf("%s(%s): ", calledFuncName, attr_value);
                        while(argument != NULL)
                        {
                            if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
                                printf("%s(%s)\t", (char*)xmlNodeGetContent(argument), attr_value);
                            argument = argument->next;
                        }
                        printf("\n");
                        ret = true;
                    }
                    
                    //handle function call as argument
                    argument = argument_list->children;
                    while(argument != NULL)
                    {
                        if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
                            ret |= literalScliceVarFromNode(literalName, argument->children, false);
                        argument = argument->next;
                    }
                }
                argument_list = argument_list->next;
            }
        }
        
        if(recursive_flag)
            ret |= literalScliceVarFromNode(literalName, cur->children, false);
        
        if(flag)
            break;
        cur = cur->next;
    }
    
    return ret;
}

bool ScliceConfKey(char *confName, char *xmlFilePath)
{
    bool ret = false;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		RecordLog(error_info);
        return ret;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return ret;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            xmlNodePtr temp_cur = funcNode->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    attr_value = getLine(temp_cur);
                    break;
                }
                temp_cur = temp_cur->next;
            }
            
            if(literalScliceVar(confName, funcNode))
            {
                printf("function: %s(%s)\n", (char*)xmlNodeGetContent(temp_cur), attr_value);
                ret = true;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);
    
    return ret;
}
