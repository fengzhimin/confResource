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
                        scanCallFunction(cur->children, (char*)xmlNodeGetContent(temp_cur), "static", src_dir);
                    else
                        scanCallFunction(cur->children, (char*)xmlNodeGetContent(temp_cur), "extern", src_dir);
                    
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

void scanCallFunction(xmlNodePtr cur, char *funcName, char *funcType, char *srcPath)
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
                    sprintf(sqlCommand, "insert into funcCall (funcName, funcCallType, sourceFile, calledFunc, line, type) \
                        value('%s', '%s', '%s', '%s', %s, 'L')", funcName, funcType, srcPath, callFuncName, attr_value);
                    if(!executeCommand(sqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                        RecordLog(error_info);
                    }
                }
            }
        }
        else
            scanCallFunction(cur->children, funcName, funcType, srcPath);
        cur = cur->next;
    }
}

void scanCallFunc(xmlNodePtr cur)
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
        else
            scanCallFunc(cur->children);
        cur = cur->next;
    }
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
                printf("%s(%s): %s\n", (char*)xmlNodeGetContent(temp_cur), attr_value, src_dir);
            }
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);  
    return begin;  
}

bool scanVarIsUsed(xmlNodePtr cur, char *varName)
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
                        printf("%s(%s) ", str, attr_value);
                        ret = true;
                    }
                }
                temp_cur = temp_cur->next;
            }
        }
        else
            ret |= scanVarIsUsed(cur->children, varName);
        
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

void ExtractVarUsedInfo(xmlNodePtr cur)
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
                temp_cur = temp_cur->next;
            }
        }
        else
            ExtractVarUsedInfo(cur->children);
            
        cur = cur->next;
    }
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
            ExtractVarUsedInfo(temp_cur);
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc); 
}

void varSclice(char *varName, xmlNodePtr cur)
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
                    
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), varName) == 0)
                        printf("%s\n", attr_value);
                }
                temp_cur = temp_cur->next;
            }
        }
        else
            varSclice(varName, cur->children);
        
        if(!xmlStrcmp(cur->name, (const xmlChar*)"if"))
        {
            xmlNodePtr temp_cur = cur->children->next->children->next->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(!xmlStrcmp(temp_cur->children->name, (const xmlChar*)"name"))
                        attr_value = xmlGetProp(temp_cur->children, (xmlChar*)"line");
                    else
                        attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), varName) == 0)
                    {
                        //打印整个if-else结构块
                        //if
                        scanCallFunc(cur->children);
                        break;
                    }
                }
                temp_cur = temp_cur->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"while"))
        {
            xmlNodePtr temp_cur = cur->children->next->children->next->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(!xmlStrcmp(temp_cur->children->name, (const xmlChar*)"name"))
                        attr_value = xmlGetProp(temp_cur->children, (xmlChar*)"line");
                    else
                        attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), varName) == 0)
                    {
                        //打印整个if-else结构块
                        //if
                        scanCallFunc(cur->children);
                        break;
                    }
                    //printf("%s(%s)\n", (char*)xmlNodeGetContent(temp_cur), attr_value);
                }
                temp_cur = temp_cur->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"for"))
        {
            xmlNodePtr temp_cur = cur->children->next->children->next->next->next->children->children;
            xmlChar* attr_value = NULL;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(!xmlStrcmp(temp_cur->children->name, (const xmlChar*)"name"))
                        attr_value = xmlGetProp(temp_cur->children, (xmlChar*)"line");
                    else
                        attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), varName) == 0)
                    {
                        //打印整个if-else结构块
                        //if
                        scanCallFunc(cur->children);
                        break;
                    }
                    //printf("%s(%s)\n", (char*)xmlNodeGetContent(temp_cur), attr_value);
                }
                temp_cur = temp_cur->next;
            }
        }
            
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
            varSclice(varName, temp_cur);
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc); 
}