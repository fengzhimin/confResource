/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CPPXmlOper.c
* Descripe     : parse C Plus Plus language xml file
******************************************************/

#include "CPPXmlOper.h"

static char error_info[LOGINFO_LENGTH];

#define scanCallFunction(tempFuncCallTableName, cur, funcName, funcType, funcArgumentType, srcPath, className, varTypeBegin)   \
    scanCallFunctionFromNode(tempFuncCallTableName, cur, funcName, funcType, funcArgumentType, srcPath, className, varTypeBegin, true)

static void scanCallFunctionFromNode(char *tempFuncCallTableName, xmlNodePtr cur, char *funcName, char *funcType, char *funcArgumentType,\
    char *srcPath, char *className, varType *varTypeBegin, bool flag);
    
bool ExtractFuncFromCPPXML(char *xmlFilePath, char *tempFuncScoreTableName, char *tempFuncCallTableName)
{
    bool ret = true;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		Error(error_info);
        xmlFreeDoc(doc);
        return false;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		Error(error_info);  
        xmlFreeDoc(doc);
        return false;
    }
    
    ret |= ExtractCPPFunc(xmlFilePath, cur, tempFuncScoreTableName, tempFuncCallTableName);
      
    xmlFreeDoc(doc);
    optDataBaseOper(tempFuncScoreTableName, tempFuncCallTableName);
    return ret;  
}

bool ExtractErrorCPPFunc(char *xmlFilePath, xmlNodePtr cur, char *tempFuncScoreTableName, char *tempFuncCallTableName)
{
    bool ret = true;
    cur = cur->children;
    char clssNameStr2[2][MAX_SUBSTR];
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl"))
        {
            xmlNodePtr temp_cur = cur->children;
            char *funcType = "extern";
            while(temp_cur != NULL)
            {
                char *value = (char*)xmlNodeGetContent(temp_cur);
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name") && temp_cur->next != NULL && !xmlStrcmp(temp_cur->next->name, (const xmlChar*)"argument_list"))
                {
                    if(strcasecmp("__attribute__", value) == 0)
                    {
                        xmlFree(value);
                        break;
                    }
                    char src_dir[DIRPATH_MAX] = "";
                    //删除开头的temp_和结尾的.xml
                    strncpy(src_dir, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                    xmlChar* attr_value = getLine(temp_cur);

                    char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
                    //get function argument type string
                    char *argumentTypeString = ExtractErrorFuncArgumentType(temp_cur->next);
                    int isClass = cutStrByLabel(value, ':', clssNameStr2, 2);
                    if(isClass != 1)
                    {
                        //是类的成员函数
                        sprintf(tempSqlCommand, "insert into %s (funcName, type, argumentType, sourceFile, line, className) value('%s', '%s', '%s', '%s', %s, '%s')", \
                            tempFuncScoreTableName, value, funcType, argumentTypeString, src_dir, attr_value, clssNameStr2[0]);
                    }
                    else
                    {
                        //不是类的成员函数
                        sprintf(tempSqlCommand, "insert into %s (funcName, type, argumentType, sourceFile, line) value('%s', '%s', '%s', '%s', %s)", \
                            tempFuncScoreTableName, value, funcType, argumentTypeString, src_dir, attr_value);
                    }

                    xmlFree(attr_value);
                    
                    if(!executeSQLCommand(NULL, tempSqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
                        Error(error_info);
                        ret = false;
                    }

                    //获取函数参数定义的变量信息
                    varType *begin = ExtractErrorVarType(temp_cur->next);
                    varType *end = begin;
                    if(begin != NULL)
                    {
                        while(end->next != NULL)
                        {
                            end = end->next;
                        }
                    }
                    
                    xmlNodePtr blockNode = temp_cur->next->next;
                    while(blockNode != NULL)
                    {
                        if(!xmlStrcmp(blockNode->name, (const xmlChar*)"argument_list"))
                            break;
                        blockNode = blockNode->next;
                    }
                    
                    //获取函数block块中定义的变量信息
                    if(end != NULL)
                        end->next = ExtractErrorVarType(blockNode);
                    else
                        begin = ExtractErrorVarType(blockNode);
                    
                    if(isClass != 1)
                        scanCallFunction(tempFuncCallTableName, blockNode, value, funcType, argumentTypeString, src_dir, clssNameStr2[0], begin);
                    else
                        scanCallFunction(tempFuncCallTableName, blockNode, value, funcType, argumentTypeString, src_dir, NULL, begin);
                    
                    varType *current = begin;
                    while(current != NULL)
                    {
                        begin = begin->next;
                        //printf("%s(%d):%s\n", current->type, current->line, current->varName);
                        free(current);
                        current = begin;
                    }
                    xmlFree(value);
                    value = NULL;
                    free(argumentTypeString);
                }
                if(value != NULL)
                    xmlFree(value);
                temp_cur = temp_cur->next;
            }
        }
        
        cur = cur->next;
    }
    
    return ret;
}

bool ExtractCPPFunc(char *xmlFilePath, xmlNodePtr cur, char *tempFuncScoreTableName, char *tempFuncCallTableName)
{
    bool ret = true;
    cur = cur->children;
    char clssNameStr2[2][MAX_SUBSTR];
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")) || \
            (!xmlStrcmp(cur->parent->name, (const xmlChar*)"block") && !xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") && cur->children != NULL && \
            !xmlStrcmp(cur->last->name, (const xmlChar*)"decl")))
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
                char *value = (char*)xmlNodeGetContent(temp_cur);
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"template"))
                    break;
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"specifier"))
                {
                    if(strcasecmp(value, "static") == 0)
                        funcType = "static";
                }
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(strcasecmp("__attribute__", value) == 0)
                    {
                        xmlFree(value);
                        break;
                    }
                    char src_dir[DIRPATH_MAX] = "";
                    //删除开头的temp_和结尾的.xml
                    strncpy(src_dir, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                    xmlChar* attr_value = getLine(temp_cur);

                    char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
                    //get function argument type string
                    char *argumentTypeString = ExtractFuncArgumentType(funcNode);
                    int isClass = cutStrByLabel(value, ':', clssNameStr2, 2);
                    if(isClass != 1)
                    {
                        //是类的成员函数
                        sprintf(tempSqlCommand, "insert into %s (funcName, type, argumentType, sourceFile, line, className) value('%s', '%s', '%s', '%s', %s, '%s')", \
                            tempFuncScoreTableName, value, funcType, argumentTypeString, src_dir, attr_value, clssNameStr2[0]);
                    }
                    else
                    {
                        //不是类的成员函数
                        sprintf(tempSqlCommand, "insert into %s (funcName, type, argumentType, sourceFile, line) value('%s', '%s', '%s', '%s', %s)", \
                            tempFuncScoreTableName, value, funcType, argumentTypeString, src_dir, attr_value);
                    }
                    xmlFree(attr_value);
                    
                    if(!executeSQLCommand(NULL, tempSqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
                        Error(error_info);
                        ret = false;
                    }

                    varType *begin = ExtractVarType(funcNode);
                    varType *current = begin;
                    
                    xmlNodePtr blockNode = temp_cur;
                    while(blockNode != NULL)
                    {
                        if(!xmlStrcmp(blockNode->name, (const xmlChar*)"block"))
                            break;
                        blockNode = blockNode->next;
                    }
                    if(isClass != 1)
                        scanCallFunction(tempFuncCallTableName, blockNode, value, funcType, argumentTypeString, src_dir, clssNameStr2[0], begin);
                    else
                        scanCallFunction(tempFuncCallTableName, blockNode, value, funcType, argumentTypeString, src_dir, NULL, begin);
                    
                    while(current != NULL)
                    {
                        begin = begin->next;
                        //printfatt("%s(%d):%s\n", current->type, current->line, current->varName);
                        free(current);
                        current = begin;
                    }
                    xmlFree(value);
                    free(argumentTypeString);
                    break;
                }
                xmlFree(value);
                temp_cur = temp_cur->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"class") && 0)
        {
            xmlNodePtr temp_cur = cur->children;
            char *className = NULL;
            char *inheritType = NULL;
            char *inheritClassName = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                    className = (char *)xmlNodeGetContent(temp_cur);
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"super"))
                {
                    xmlNodePtr inherit = temp_cur->children;
                    while(inherit != NULL)
                    {
                        if(!xmlStrcmp(inherit->name, (const xmlChar*)"specifier"))
                            inheritType = (char *)xmlNodeGetContent(inherit);
                        if(!xmlStrcmp(inherit->name, (const xmlChar*)"name"))
                        {
                            inheritClassName = (char *)xmlNodeGetContent(inherit);
                            char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
                            sprintf(tempSqlCommand, "insert into %s (className, inheritType, inheritClassName) value('%s', '%s', '%s')", \
                            classInheritTableName, className, inheritType, inheritClassName);
                            xmlFree(className);
                            xmlFree(inheritType);
                            xmlFree(inheritClassName);
                            MYSQL temp_db;
                            MYSQL *tempMysqlConnect = NULL;
                            tempMysqlConnect = mysql_init(&temp_db);
                            if(tempMysqlConnect == NULL)
                            {
                                Error("init mysql failure\n");
                                return false;
                            }
                            if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
                            {
                                memset(error_info, 0, LOGINFO_LENGTH);
                                sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
                                Error(error_info);
                                mysql_close(tempMysqlConnect);
                                return false;
                            }
                            //跳过重复插入数据错误信息
                            mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand));
                            mysql_close(tempMysqlConnect);
                        }
                        
                        inherit = inherit->next;
                    }
                    
                    break;
                }
                temp_cur = temp_cur->next;
            }
            
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL)
        {
            //handle extern "C"
            xmlNodePtr children = cur->children;
            while(children != NULL)
            {
                if(!xmlStrcmp(children->name, (const xmlChar*)"block"))
                    ret = ExtractCPPFunc(xmlFilePath, children, tempFuncScoreTableName, tempFuncCallTableName);
                else if(!xmlStrcmp(children->name, (const xmlChar*)"decl_stmt"))
                    ret = ExtractErrorCPPFunc(xmlFilePath, children, tempFuncScoreTableName, tempFuncCallTableName);
                
                children = children->next;
            }
        }
        cur = cur->next;
    }
    
    return ret;
}

/*******************************
 * func: self-define function scan call function
 * return: void
 * @para cur: current self-define function Node
 * @para funcName: current self-define function name
 * @para funcType: current self-define function type(extern or static)
 * @para srcPath: function source file path
 * @para className: 函数所在类名
 * @para varTypeBegin: variable type header point
********************************/
static void scanCallFunctionFromNode(char *tempFuncCallTableName, xmlNodePtr cur, char *funcName, char *funcType, char *funcArgumentType, \
    char *srcPath, char *className, varType *varTypeBegin, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                xmlChar* attr_value = getLine(cur->children);
                char callFuncName[128] = {};
                if(!xmlStrcmp(cur->children->children->name, (const xmlChar*)"text"))
                {
                    char *value = (char*)xmlNodeGetContent(cur->children);
                    strcat(callFuncName, value);
                    xmlFree(value);
                }
                else
                {
                    char *classSymbol = (char*)xmlNodeGetContent(cur->children->last->prev);
                    if(strcasecmp(classSymbol, "::") != 0)
                    {
                        varType *current = varTypeBegin;
                        char *varName = (char*)xmlNodeGetContent(cur->children->last->prev->prev);
                        //handle new List<Index_hint>();
                        if(varName == NULL)
                            varName = (char*)xmlNodeGetContent(cur->children->last->prev);
                        while(current != NULL)
                        {
                            if(strcasecmp(current->varName, varName) == 0)
                            {
                                sprintf(callFuncName, "%s::", current->type);
                                break;
                            }
                            current = current->next;
                        }
                        xmlFree(varName);
                        char *value = (char*)xmlNodeGetContent(cur->children->last);
                        strcat(callFuncName, value);
                        xmlFree(value);
                    }
                    else
                    {
                        char *value = (char*)xmlNodeGetContent(cur->children);
                        strcat(callFuncName, value);
                        xmlFree(value);
                    }
                    xmlFree(classSymbol);
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
                    if(className != NULL)
                    {
                        sprintf(tempSqlCommand, "insert into %s (funcName, funcCallType, argumentType, sourceFile, calledFunc, calledFuncArgumentType, \
                            CalledSrcFile, line, type, forNum, whileNum, className) value('%s', '%s', '%s', '%s', '%s', '%s', '%s', %s, 'L', %d, %d, '%s')", \
                            tempFuncCallTableName, funcName, funcType, funcArgumentType, srcPath, callFuncName, calledFuncArgumentTypeString, srcPath, \
                            attr_value, forNum, whileNum, className);
                    }
                    else
                    {
                        sprintf(tempSqlCommand, "insert into %s (funcName, funcCallType, argumentType, sourceFile, calledFunc, calledFuncArgumentType, \
                            CalledSrcFile, line, type, forNum, whileNum) value('%s', '%s', '%s', '%s', '%s', '%s', '%s', %s, 'L', %d, %d)", \
                            tempFuncCallTableName, funcName, funcType, funcArgumentType, srcPath, callFuncName, calledFuncArgumentTypeString, srcPath, \
                            attr_value, forNum, whileNum);
                    }
                    
                    free(calledFuncArgumentTypeString);
                    if(!executeSQLCommand(NULL, tempSqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
                        Error(error_info);
                        
                        return ;
                    }
                }
                xmlFree(attr_value);
            }
        }
        
        scanCallFunctionFromNode(tempFuncCallTableName, cur->children, funcName, funcType, funcArgumentType, srcPath, className, varTypeBegin, false);
        if(flag)
            break;
        cur = cur->next;
    }
}

funcInfoList *scanCPPCallFuncFromNode(xmlNodePtr cur, varType *varTypeBegin, bool flag)
{
    funcInfoList *begin = NULL;
    funcInfoList *end = NULL;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                xmlChar* attr_value = getLine(cur->children);
                char calledFuncName[128] = {};
                if(!xmlStrcmp(cur->children->children->name, (const xmlChar*)"text"))
                {
                    char *value = (char*)xmlNodeGetContent(cur->children);
                    strcat(calledFuncName, value);
                    xmlFree(value);
                }
                else
                {
                    char *classSymbol = (char*)xmlNodeGetContent(cur->children->last->prev);
                    if(strcasecmp(classSymbol, "::") != 0)
                    {
                        varType *current = varTypeBegin;
                        char *varName = (char*)xmlNodeGetContent(cur->children->last->prev->prev);
                        //handle new List<Index_hint>();
                        if(varName == NULL)
                            varName = (char*)xmlNodeGetContent(cur->children->last->prev);
                        while(current != NULL)
                        {
                            if(strcasecmp(current->varName, varName) == 0)
                            {
                                sprintf(calledFuncName, "%s::", current->type);
                                break;
                            }
                            current = current->next;
                        }
                        xmlFree(varName);
                        char *value = (char*)xmlNodeGetContent(cur->children->last);
                        strcat(calledFuncName, value);
                        xmlFree(value);
                    }
                    else
                    {
                        char *value = (char*)xmlNodeGetContent(cur->children);
                        strcat(calledFuncName, value);
                        xmlFree(value);
                    }
                    xmlFree(classSymbol);
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
                sprintf(tempSqlCommand, "select calledFuncType, calledFuncArgumentType, CalledSrcFile, type from %s where calledFunc='%s' and line=%s",\
                    funcCallTableName, calledFuncName, (char *)attr_value);
                MYSQL temp_db;
                MYSQL *tempMysqlConnect = NULL;
                tempMysqlConnect = mysql_init(&temp_db);
                if(tempMysqlConnect == NULL)
                {
                    Error("init mysql failure\n");
                    return false;
                }
                if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
                {
                    memset(error_info, 0, LOGINFO_LENGTH);
                    sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
                    Error(error_info);
                    mysql_close(tempMysqlConnect);
                    return false;
                }
                if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
                {
                    memset(error_info, 0, LOGINFO_LENGTH);
                    sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
                    Error(error_info);
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
                        //self-define function
                        while((sqlrow = mysql_fetch_row(res_ptr)) != NULL)
                        {
                            strcpy(end->info.funcType, sqlrow[0]);
                            strcpy(end->info.argumentType, sqlrow[1]);
                            strcpy(end->info.sourceFile, sqlrow[2]);
                            //sprintf(end->info.sourceFile, "temp_%s.xml", sqlrow[2]);
                            end->info.type = ((char *)sqlrow[3])[0];
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
                    mysql_free_result(res_ptr);
                }
                xmlFree(attr_value);
                mysql_close(tempMysqlConnect);
            }
        }
        
        funcInfoList *current = scanCPPCallFuncFromNode(cur->children, varTypeBegin, false);
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

funcInfoList *scanBackCPPCallFunc(xmlNodePtr cur, varType *varTypeBegin)
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
        current = scanCPPCallFunc(cur, varTypeBegin);
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
    
    current = scanBackCPPCallFunc(temp_cur->parent, varTypeBegin);
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

bool ExtractClassInheritFromCPPXML(char *docName)
{
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(docName);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", docName);
		Error(error_info);
        return false;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", docName);
		Error(error_info);  
        xmlFreeDoc(doc);
        return false;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"class"))
        {
            xmlNodePtr temp_cur = cur->children;
            char *className = NULL;
            char *inheritType = NULL;
            char *inheritClassName = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                    className = (char *)xmlNodeGetContent(temp_cur);
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"super"))
                {
                    xmlNodePtr inherit = temp_cur->children;
                    while(inherit != NULL)
                    {
                        if(!xmlStrcmp(inherit->name, (const xmlChar*)"specifier"))
                            inheritType = (char *)xmlNodeGetContent(inherit);
                        if(!xmlStrcmp(inherit->name, (const xmlChar*)"name"))
                        {
                            inheritClassName = (char *)xmlNodeGetContent(inherit);
                            printf("%s, %s, %s", className, inheritType, inheritClassName);
                            xmlFree(className);
                            xmlFree(inheritType);
                            xmlFree(inheritClassName);
                            break;
                        }
                        
                        inherit = inherit->next;
                    }
                    
                    break;
                }
                temp_cur = temp_cur->next;
            }
            
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);

    return true;  
}

funcInfoList *varCPPScliceFuncFromNode(varDef varInfo, xmlNodePtr cur, varType *varTypeBegin, bool flag)
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
            char *value = (char *)getLine(cur);
            currentLine = StrToInt(value);
            xmlFree(value);
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
                            current = scanCPPCallFunc(cur, varTypeBegin);                       
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
                                current = scanBackCPPCallFunc(cur, varTypeBegin);
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
                                        current = scanBackCPPCallFunc(cur, varTypeBegin);
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
            char *value = (char *)getLine(cur);
            currentLine = StrToInt(value);
            xmlFree(value);
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
                            current = scanCPPCallFunc(cur, varTypeBegin);                       
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
                                current = scanBackCPPCallFunc(cur, varTypeBegin);
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
                                        current = scanBackCPPCallFunc(cur, varTypeBegin);
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
            char *value = (char *)getLine(cur);
            currentLine = StrToInt(value);
            xmlFree(value);
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
                            current = scanCPPCallFunc(cur, varTypeBegin);                       
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
                                current = scanBackCPPCallFunc(cur, varTypeBegin);
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
                                        current = scanBackCPPCallFunc(cur, varTypeBegin);
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
            char *value = (char *)getLine(cur);
            currentLine = StrToInt(value);
            xmlFree(value);
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
                                    current = scanCPPCallFunc(cur, varTypeBegin);                       
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
                                        current = scanBackCPPCallFunc(cur, varTypeBegin);
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
                                                current = scanBackCPPCallFunc(cur, varTypeBegin);
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
            char calledFuncName[128] = {};
            while(argument_list != NULL)
            {
                if(!xmlStrcmp(argument_list->name, (const xmlChar*)"name"))
                {
                    if(cur->children->last != NULL)
                    {
                        if(!xmlStrcmp(cur->children->children->name, (const xmlChar*)"text"))
                        {
                            char *value = (char*)xmlNodeGetContent(cur->children);
                            strcat(calledFuncName, value);
                            xmlFree(value);
                        }
                        else
                        {
                            char *classSymbol = (char*)xmlNodeGetContent(cur->children->last->prev);
                            if(strcasecmp(classSymbol, "::") != 0)
                            {
                                varType *currentVarType = varTypeBegin;
                                char *varName = (char*)xmlNodeGetContent(cur->children->last->prev->prev);
                                //handle new List<Index_hint>();
                                if(varName == NULL)
                                    varName = (char*)xmlNodeGetContent(cur->children->last->prev);
                                while(currentVarType != NULL)
                                {
                                    if(strcasecmp(currentVarType->varName, varName) == 0)
                                    {
                                        sprintf(calledFuncName, "%s::", currentVarType->type);
                                        break;
                                    }
                                    currentVarType = currentVarType->next;
                                }
                                xmlFree(varName);
                                char *value = (char*)xmlNodeGetContent(cur->children->last);
                                strcat(calledFuncName, value);
                                xmlFree(value);
                            }
                            else
                            {
                                char *value = (char*)xmlNodeGetContent(cur->children);
                                strcat(calledFuncName, value);
                                xmlFree(value);
                            }
                            xmlFree(classSymbol);
                        }
                    }
                    else
                    {
                        char *value = (char*)xmlNodeGetContent(argument_list);
                        strcpy(calledFuncName, value);
                        xmlFree(value);
                    }
                    if(attr_value != NULL)
                        xmlFree(attr_value);
                    attr_value = NULL;
                    attr_value = getLine(argument_list);
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
                        sprintf(tempSqlCommand, "select calledFuncType, calledFuncArgumentType, CalledSrcFile, type from %s where calledFunc='%s' and line=%s",\
                            funcCallTableName, calledFuncName, (char *)attr_value);
                        MYSQL temp_db;
                        MYSQL *tempMysqlConnect = NULL;
                        tempMysqlConnect = mysql_init(&temp_db);
                        if(tempMysqlConnect == NULL)
                        {
                            Error("init mysql failure\n");
                            return false;
                        }
                        if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
                        {
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
                            Error(error_info);
                            mysql_close(tempMysqlConnect);
                            return false;
                        }
                        if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
                        {
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
                            Error(error_info);
                            mysql_close(tempMysqlConnect);
                            return false;
                        }
                        else
                        {
                            MYSQL_RES *res_ptr;
                            MYSQL_ROW sqlrow;
                            res_ptr = mysql_store_result(tempMysqlConnect);
                            int rownum = mysql_num_rows(res_ptr);
                            //count 为递归的最大深度
                            if(rownum != 0)
                            {
                                //self-define function
                                while((sqlrow = mysql_fetch_row(res_ptr)) != NULL)
                                {
                                    strcpy(end->info.funcType, sqlrow[0]);
                                    strcpy(end->info.argumentType, sqlrow[1]);
                                    strcpy(end->info.sourceFile, sqlrow[2]);
                                    //sprintf(end->info.sourceFile, "temp_%s.xml", sqlrow[2]);
                                    end->info.type = ((char *)sqlrow[3])[0];
                                    
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
                            mysql_free_result(res_ptr);
                        }
                        mysql_close(tempMysqlConnect);
                    }
                    
                    //handle function call as argument
                    xmlNodePtr argument = argument_list->children;
                    while(argument != NULL)
                    {
                        if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
                        {
                            current = varCPPScliceFuncFromNode(varInfo, argument->children, varTypeBegin, false);
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
            if(attr_value != NULL)
                xmlFree(attr_value);
        }
        
        if(recursive_flag)
        {
            current = varCPPScliceFuncFromNode(varInfo, cur->children, varTypeBegin, false);
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

funcCallInfoList *CPPSclice(char *varName, char *xmlFilePath)
{
    return Sclice(varName, xmlFilePath, varCPPScliceFuncFromNode);
}

varDirectInflFuncList *getCPPDirectInflFuncFromNode(char *varName, xmlNodePtr funcBlockNode, varType *varTypeBegin, bool flag)
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
            char calledFuncName[128] = {};
            while(argument_list != NULL)
            {
                if(!xmlStrcmp(argument_list->name, (const xmlChar*)"name"))
                {
                    if(funcBlockNode->children->last != NULL)
                    {
                        if(!xmlStrcmp(funcBlockNode->children->children->name, (const xmlChar*)"text"))
                        {
                            char *value = (char*)xmlNodeGetContent(funcBlockNode->children);
                            strcat(calledFuncName, value);
                            xmlFree(value);
                        }
                        else
                        {
                            char *classSymbol = (char*)xmlNodeGetContent(funcBlockNode->children->last->prev);
                            if(strcasecmp(classSymbol, "::") != 0)
                            {
                                varType *currentVarType = varTypeBegin;
                                char *tempName = (char*)xmlNodeGetContent(funcBlockNode->children->last->prev->prev);
                                //handle new List<Index_hint>();
                                if(tempName == NULL)
                                    tempName = (char*)xmlNodeGetContent(funcBlockNode->children->last->prev);
                                while(currentVarType != NULL)
                                {
                                    if(strcasecmp(currentVarType->varName, tempName) == 0)
                                    {
                                        sprintf(calledFuncName, "%s::", currentVarType->type);
                                        break;
                                    }
                                    currentVarType = currentVarType->next;
                                }
                                xmlFree(tempName);
                                char *value = (char*)xmlNodeGetContent(funcBlockNode->children->last);
                                strcat(calledFuncName, value);
                                xmlFree(value);
                            }
                            else
                            {
                                char *value = (char*)xmlNodeGetContent(funcBlockNode->children);
                                strcat(calledFuncName, value);
                                xmlFree(value);
                            }
                            xmlFree(classSymbol);
                        }
                    }
                    else
                    {
                        char *value = (char*)xmlNodeGetContent(argument_list);
                        strcpy(calledFuncName, value);
                        xmlFree(value);
                    }
                    if(attr_value != NULL)
                        xmlFree(attr_value);
                    attr_value = NULL;
                    attr_value = getLine(argument_list);
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
                            Error("init mysql failure\n");
                            return NULL;
                        }
                        if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
                        {
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
                            Error(error_info);
                            mysql_close(tempMysqlConnect);
                            return NULL;
                        }
                        if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
                        {
                            memset(error_info, 0, LOGINFO_LENGTH);
                            sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
                            Error(error_info);
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
                                //self-define function
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
                                    
                                    break;
                                }
                            }
                            mysql_free_result(res_ptr);
                        }
                        mysql_close(tempMysqlConnect);
                    }
                }
                argument_list = argument_list->next;
            }
            if(attr_value != NULL)
                xmlFree(attr_value);
        }
        current = getCPPDirectInflFuncFromNode(varName, funcBlockNode->children, varTypeBegin, false);
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

varDirectInflFuncList *getCPPVarInfluFunc(char *varName, char *funcName, char *xmlFilePath, char *funcArgumentType)
{
    return getVarInfluFunc(varName, funcName, xmlFilePath, funcArgumentType, getCPPDirectInflFuncFromNode);
}