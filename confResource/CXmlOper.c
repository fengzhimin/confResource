/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CXmlOper.c
* Descripe     : parse C language xml file
******************************************************/

#include "CXmlOper.h"

static char error_info[LOGINFO_LENGTH];
static char src_dir[DIRPATH_MAX];
static char sqlCommand[LINE_CHAR_MAX_NUM];

bool ExtractFuncFromXML(char *docName)
{
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(docName);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", docName);
		RecordLog(error_info);
        return false;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", docName);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return false;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
        {
            xmlNodePtr temp_cur = cur->children;
            bool funcType = false;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"specifier"))
                {
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), "static") == 0)
                        funcType = true;
                }
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    memset(src_dir, 0, DIRPATH_MAX);
                    //删除开头的temp_和结尾的.xml
                    strncpy(src_dir, (char *)&(docName[5]), strlen(docName)-9);
                    xmlChar* attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                    if(funcType)
                        sprintf(sqlCommand, "insert into funcScore (funcName, type, sourceFile, line) value('%s', 'static', '%s', %s)", \
                        (char*)xmlNodeGetContent(temp_cur), src_dir, attr_value);
                    else
                        sprintf(sqlCommand, "insert into funcScore (funcName, sourceFile, line) value('%s', '%s', %s)", \
                        (char*)xmlNodeGetContent(temp_cur), src_dir, attr_value);
                        
                    if(!executeCommand(sqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                        RecordLog(error_info);
                    }
                    if(funcType)
                        scanCallFunction(cur, (char*)xmlNodeGetContent(temp_cur), "static", src_dir);
                    else
                        scanCallFunction(cur, (char*)xmlNodeGetContent(temp_cur), "extern", src_dir);
                    
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

void scanCallFunctionFromNode(xmlNodePtr cur, char *funcName, char *funcType, char *srcPath, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                xmlChar* attr_value = NULL;
                char *callFuncName = NULL;
                if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                {
                    attr_value = xmlGetProp(cur->children->last, (xmlChar*)"line");
                    callFuncName = (char*)xmlNodeGetContent(cur->children->last);
                }
                else
                {
                    attr_value = xmlGetProp(cur->children, (xmlChar*)"line");
                    callFuncName = (char*)xmlNodeGetContent(cur->children);
                }
                //删除递归调用
                if(strcasecmp(callFuncName, funcName) != 0)
                {
                    sprintf(sqlCommand, "insert into funcCall (funcName, funcCallType, sourceFile, calledFunc, CalledSrcFile, line, type) \
                        value('%s', '%s', '%s', '%s', '%s', %s, 'L')", funcName, funcType, srcPath, callFuncName, srcPath, attr_value);
                    if(!executeCommand(sqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                        RecordLog(error_info);
                    }
                }
            }
        }
        
        scanCallFunctionFromNode(cur->children, funcName, funcType, srcPath, false);
        if(flag)
            break;
        cur = cur->next;
    }
}

void scanCallFuncFromNode(xmlNodePtr cur, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                xmlChar* attr_value = NULL;
                char *callFuncName = NULL;
                if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                {
                    attr_value = xmlGetProp(cur->children->last, (xmlChar*)"line");
                    callFuncName = (char*)xmlNodeGetContent(cur->children->last);
                }
                else
                {
                    attr_value = xmlGetProp(cur->children, (xmlChar*)"line");
                    callFuncName = (char*)xmlNodeGetContent(cur->children);
                }
                
                printf("%s(%s)\n", callFuncName, attr_value);
            }
        }
        
        scanCallFuncFromNode(cur->children, false);
        
        if(flag)
            break;
        cur = cur->next;
    }
}

void scanAssignVarFromNode(xmlNodePtr cur, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"expr"))
        {
            xmlNodePtr opt = cur->children;
            while(opt != NULL)
            {
                if(!xmlStrcmp(opt->name, (const xmlChar*)"operator"))
                { 
                    char *optName = (char*)xmlNodeGetContent(opt);
                    if(strcasecmp(optName, "=") == 0)
                    {
                        xmlNodePtr name = opt->prev;
                        while(name != NULL)
                        {
                            if(!xmlStrcmp(name->name, (const xmlChar*)"name"))
                            {
                                char *varName = (char*)xmlNodeGetContent(name);
                                xmlChar* attr_value = NULL;
                                if(!xmlStrcmp(name->children->name, (const xmlChar*)"name"))
                                    attr_value = xmlGetProp(name->children, (xmlChar*)"line");
                                else
                                    attr_value = xmlGetProp(name, (xmlChar*)"line");
                                printf("%s(%s)\n", varName, attr_value);
                                break;
                            }
                            name = name->prev;
                        }
                    }
                }

                opt = opt->next;
            }
        }
        
        scanAssignVarFromNode(cur->children, false);
        
        if(flag)
            break;
        cur = cur->next;
    }
}

void scanBackCallFunc(xmlNodePtr cur)
{
    if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
    {
        return ;
    }
    xmlNodePtr temp_cur = cur;
    cur = cur->next;
    while(cur != NULL)
    {
        scanCallFunc(cur);
        cur = cur->next;
    }
    
    scanBackCallFunc(temp_cur->parent);
}

void scanBackAssignVar(xmlNodePtr cur)
{
    if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
    {
        return ;
    }
    xmlNodePtr temp_cur = cur;
    cur = cur->next;
    while(cur != NULL)
    {
        scanAssignVar(cur);
        cur = cur->next;
    }
    
    scanBackAssignVar(temp_cur->parent);
}

funcList *ExtractVarUsedFunc(char *varName, char *xmlFilePath)
{
    funcList *begin = NULL;
    funcList *end = NULL;
    xmlDocPtr doc;
    xmlNodePtr cur;
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(xmlFilePath);
    if(doc == NULL )
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", xmlFilePath);
		RecordLog(error_info);
        return NULL;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return NULL;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;
            bool funcType = false;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"specifier"))
                {
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), "static") == 0)
                        funcType = true;
                }
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    /*针对ClassName::funcName类情况
                    if(attr_value == NULL)
                        attr_value = xmlGetProp(temp_cur->next, (xmlChar*)"line");
                    */
                    break;
                }
                temp_cur = temp_cur->next;
            }
            //对函数体分析
            //block
            if(scanVarIsUsed(temp_cur->next->next, varName))
            {
                memset(src_dir, 0, DIRPATH_MAX);
                //删除开头的temp_和结尾的.xml
                strncpy(src_dir, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
                if(begin == NULL)
                    begin = end = malloc(sizeof(funcList));
                else
                    end = end->next = malloc(sizeof(funcList));
                memset(end, 0, sizeof(funcList));
                if(strlen((char*)xmlNodeGetContent(temp_cur)) < MAX_FUNCNAME_LENGTH)
                {
                    strcpy(end->funcName, (char*)xmlNodeGetContent(temp_cur));
                    end->line = StrToInt((char *)attr_value);
                    end->funcType = funcType;
                    strcpy(end->sourceFile, src_dir);
                }
                else
                {
                    memset(error_info, 0, LOGINFO_LENGTH);
                    sprintf(error_info, "%s:%s(%s) function name length more than %d\n", src_dir, (char*)xmlNodeGetContent(temp_cur), attr_value, MAX_FUNCNAME_LENGTH);
                    RecordLog(error_info);
                }
                //printf("%s(%s): %s\n", (char*)xmlNodeGetContent(temp_cur), attr_value, src_dir);
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);  
    return begin;  
}

bool scanVarIsUsedFromNode(xmlNodePtr cur, char *varName, bool flag)
{ 
    bool ret = false;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"expr"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(!xmlStrcmp(temp_cur->children->name, (const xmlChar*)"name"))
                        attr_value = xmlGetProp(temp_cur->children, (xmlChar*)"line");
                    else
                        attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    char *str = (char*)xmlNodeGetContent(temp_cur);
                    if(strcasecmp(str, varName) == 0)
                    {
                        //printf("%s(%s) ", str, attr_value);
                        ret = true;
                    }
                }
                else
                    ret |= scanVarIsUsedFromNode(temp_cur->children, varName, false);
                temp_cur = temp_cur->next;
            }
        }
        else
            ret |= scanVarIsUsedFromNode(cur->children, varName, false);
        
        if(flag)
            break;
        cur = cur->next;
    }
    
    return ret;
}

//example static struct stu *stu1, *stu2, stu3;
//type = static struct stu
varType *ExtractVarDefFromNode(xmlNodePtr cur, bool flag)
{
    varType *begin, *end;
    begin = end = NULL;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt"))
        {
            if(begin == NULL)
                begin = end = malloc(sizeof(varType));
            else
                end = end->next = malloc(sizeof(varType));
            memset(end, 0, sizeof(varType));
            
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = xmlGetProp(cur->last, (xmlChar*)"line");
            char type[MAX_VARIABLE_LENGTH];
            memset(type, 0, MAX_VARIABLE_LENGTH);
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"decl"))
                {
                    if(temp_cur->prev == NULL)
                    {
                        xmlNodePtr temp = temp_cur->children;
                        while(temp != NULL)
                        {
                            //handle static
                            if(!xmlStrcmp(temp->name, (const xmlChar*)"specifier"))
                                strcat(type, (char*)xmlNodeGetContent(temp));
                            //struct stu *
                            if(!xmlStrcmp(temp->name, (const xmlChar*)"type"))
                            {
                                xmlNodePtr temp_type = temp->children;
                                while(temp_type != NULL)
                                {
                                    //handle struct stu
                                    if(!xmlStrcmp(temp_type->name, (const xmlChar*)"name"))
                                    {
                                        if(!xmlStrcmp(temp_type->children->name, (const xmlChar*)"text"))
                                        {
                                            if(strlen(type) != 0)
                                                strcat(type, " ");
                                            strcat(type, (char*)xmlNodeGetContent(temp_type));
                                        }
                                        else
                                        {
                                            xmlNodePtr temp_name = temp_type->children;
                                            while(temp_name != NULL)
                                            {
                                                if(strlen(type) != 0)
                                                    strcat(type, " ");
                                                strcat(type, (char*)xmlNodeGetContent(temp_name));
                                                temp_name = temp_name->next;
                                            }
                                        }
                                    }
                                    //handle *
                                    if(!xmlStrcmp(temp_type->name, (const xmlChar*)"modifier"))
                                    {
                                        strcat(end->type, type);
                                        strcat(end->type, " ");
                                        strcat(end->type, (char*)xmlNodeGetContent(temp_type));
                                    }
                                    temp_type = temp_type->next;
                                }
                            }
                            //handle stu1
                            if(!xmlStrcmp(temp->name, (const xmlChar*)"name"))
                            {
                                strcat(end->varName, (char*)xmlNodeGetContent(temp));
                                end->line = StrToInt(attr_value);
                                if(strlen(end->type) == 0)
                                {
                                    strcat(end->type, type);
                                    strcat(end->type, " ");
                                }
                            }
                            temp = temp->next;
                        }
                    }
                    else
                    {
                        //handle stu3
                        end = end->next = malloc(sizeof(varType));
                        memset(end, 0, sizeof(varType));
                        strcat(end->type, type);
                        end->line = StrToInt(attr_value);
                        strcat(end->varName, (char*)xmlNodeGetContent(temp_cur->children->next));
                    }
                }
                else if(xmlStrcmp(temp_cur->name, (const xmlChar*)"text") && xmlStrcmp(temp_cur->name, (const xmlChar*)"position"))
                {
                    end = end->next = malloc(sizeof(varType));
                    memset(end, 0, sizeof(varType));
                    //handle *stu2
                    if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"modifier"))
                    {
                        strcat(end->type, type);
                        strcat(end->type, " ");
                        strcat(end->type, (char*)xmlNodeGetContent(temp_cur));
                        end->line = StrToInt(attr_value);
                        temp_cur = temp_cur->next;
                        strcat(end->varName, (char*)xmlNodeGetContent(temp_cur));
                    }
                }
                temp_cur = temp_cur->next;
            }
        }
        else
        {
            if(begin != NULL)
                end->next = ExtractVarDefFromNode(cur->children, false);
            else
                begin = end = ExtractVarDefFromNode(cur->children, false);
        }
        
        if(flag)
            break;
        
        cur = cur->next;
    }
    
    return begin;
}

void ExtractFuncVarDef(char *xmlFilePath)
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
        return ;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return ;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    printf("function: %s(%s)\n", (char*)xmlNodeGetContent(temp_cur), attr_value);
                    break;
                }
                temp_cur = temp_cur->next;
            }
            varType *begin = ExtractVarDef(cur);
            varType *current = begin;
            while(current != NULL)
            {
                begin = begin->next;
                printf("%s %s(%d)\n", current->type, current->varName, current->line);
                free(current);
                current = begin;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc); 
}

void ExtractGlobalVarDef(char *xmlFilePath)
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
        return ;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return ;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt"))
        {
            printf("global variable define info: \n");
            varType *begin = ExtractVarDef(cur);
            varType *current = begin;
            while(current != NULL)
            {
                begin = begin->next;
                printf("%s %s(%d)\n", current->type, current->varName, current->line);
                free(current);
                current = begin;
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc); 
}

void ExtractVarUsedInfoFromNode(xmlNodePtr cur, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"expr"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(!xmlStrcmp(temp_cur->children->name, (const xmlChar*)"name"))
                        attr_value = xmlGetProp(temp_cur->children, (xmlChar*)"line");
                    else
                        attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    
                    printf("%s(%s)\n", (char*)xmlNodeGetContent(temp_cur), attr_value);
                }
                else
                    ExtractVarUsedInfoFromNode(temp_cur->children, false);
                temp_cur = temp_cur->next;
            }
        }
        else
            ExtractVarUsedInfoFromNode(cur->children, false);
        
        if(flag)
            break;
        cur = cur->next;
    }
}

bool JudgeVarUsedFromNode(xmlNodePtr cur, char *var, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"expr"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(!xmlStrcmp(temp_cur->children->name, (const xmlChar*)"name"))
                        attr_value = xmlGetProp(temp_cur->children, (xmlChar*)"line");
                    else
                        attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                        
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), var) == 0)
                        return true;
                }
                
                temp_cur = temp_cur->next;
            }
        }
        
        if(JudgeVarUsedFromNode(cur->children, var, false))
            return true;
        if(flag)
            break;
        cur = cur->next;
    }
    
    return false;
}

void ExtractFuncVarUsedInfo(char *xmlFilePath)
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
        return ;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return ;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    printf("function: %s(%s)\n", (char*)xmlNodeGetContent(temp_cur), attr_value);
                    break;
                }
                temp_cur = temp_cur->next;
            }
            //function block
            ExtractVarUsedInfo(temp_cur->next->next);
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc); 
}

bool JudgeExistChildNodeFromNode(xmlNodePtr cur, char *nodeName, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)nodeName))
            return true;
        else if(JudgeExistChildNodeFromNode(cur->children, nodeName, false))
            return true;
        if(flag)
            break;
        cur = cur->next;
    }
    
    return false;
}

void varScliceFuncFromNode(char *varName, xmlNodePtr cur, bool flag)
{
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
                    //判断if条件中是否使用了varName变量，如果使用则该变量影响整个if块
                    if(JudgeVarUsed(condition, varName))
                    {
                        //打印整个if-else结构块
                        scanCallFunc(cur);
                        scanAssignVar(cur);
                        xmlNodePtr then = condition->next;
                        while(then != NULL)
                        {
                            if(JudgeExistChildNode(then, "return"))
                            {
                                scanBackCallFunc(cur);
                                scanBackAssignVar(cur);
                                break;
                            }
                            then = then->next;
                        }
                        recursive_flag = false;
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
                    //判断while条件中是否使用了varName变量，如果使用则该变量影响整个while块
                    if(JudgeVarUsed(condition, varName))
                    {
                        //打印整个while结构块
                        scanCallFunc(cur);
                        scanAssignVar(cur);
                        xmlNodePtr block = condition->next;
                        while(block != NULL)
                        {
                            if(JudgeExistChildNode(block, "return"))
                            {
                                scanBackCallFunc(cur);
                                scanBackAssignVar(cur);
                                break;
                            }
                            block = block->next;
                        }
                        recursive_flag = false;
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
                    //判断do-while条件中是否使用了varName变量，如果使用则该变量影响整个do-while块
                    if(JudgeVarUsed(condition, varName))
                    {
                        //打印整个do-while结构块
                        scanCallFunc(cur);
                        scanAssignVar(cur);
                        xmlNodePtr block = condition->prev;
                        while(block != NULL)
                        {
                            if(JudgeExistChildNode(block, "return"))
                            {
                                scanBackCallFunc(cur);
                                scanBackAssignVar(cur);
                                break;
                            }
                            block = block->prev;
                        }
                        recursive_flag = false;
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
                        //判断for条件中是否使用了varName变量，如果使用则该变量影响整个for块
                        if(!xmlStrcmp(condition->name, (const xmlChar*)"condition"))
                        {
                            if(JudgeVarUsed(condition, varName))
                            {
                                //打印整个for结构块
                                scanCallFunc(cur);
                                scanAssignVar(cur);
                                xmlNodePtr block = control->next;
                                while(block != NULL)
                                {
                                    if(JudgeExistChildNode(block, "return"))
                                    {
                                        scanBackCallFunc(cur);
                                        scanBackAssignVar(cur);
                                        break;
                                    }
                                    block = block->next;
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
                    attr_value = xmlGetProp(argument_list, (xmlChar*)"line");
                    calledFuncName = (char*)xmlNodeGetContent(argument_list);
                }
                else if(!xmlStrcmp(argument_list->name, (const xmlChar*)"argument_list"))
                {
                    if(scanVarIsUsed(argument_list, varName))
                        printf("%s(%s)\n", calledFuncName, attr_value);
                    
                    //handle function call as argument
                    xmlNodePtr argument = argument_list->children;
                    while(argument != NULL)
                    {
                        if(!xmlStrcmp(argument->name, (const xmlChar*)"argument"))
                            varScliceFuncFromNode(varName, argument->children, false);
                        argument = argument->next;
                    }
                }
                argument_list = argument_list->next;
            }
        }
        
        if(recursive_flag)
            varScliceFuncFromNode(varName, cur->children, false);
        
        if(flag)
            break;
        cur = cur->next;
    }
}

void Sclice(char *varName, char *xmlFilePath)
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
        return ;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return ;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    printf("function: %s(%s)\n", (char*)xmlNodeGetContent(temp_cur), attr_value);
                    break;
                }
                temp_cur = temp_cur->next;
            }
            varScliceFunc(varName, cur);
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc); 
}

bool ExtractConfKeyUsedInfoFromNode(xmlNodePtr cur, char *confName, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"literal"))
        {
            char *content = xmlNodeGetContent(cur);
            char *type = xmlGetProp(cur, (xmlChar*)"type");
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
                    attr_value = xmlGetProp(argument_list, (xmlChar*)"line");
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
        return ;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "empty document(%s). \n", xmlFilePath);
		RecordLog(error_info);  
        xmlFreeDoc(doc);
        return ;
    }
    
    cur = cur->children;
    while (cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
        {
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    break;
                }
                temp_cur = temp_cur->next;
            }
            
            if(literalScliceVar(confName, cur))
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