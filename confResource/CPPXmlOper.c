/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CPPXmlOper.c
* Descripe     : parse C Plus Plus language xml file
******************************************************/

#include "CPPXmlOper.h"

static char error_info[LOGINFO_LENGTH];
static char src_dir[DIRPATH_MAX];
static char sqlCommand[LINE_CHAR_MAX_NUM];

#define scanCallFunction(cur, funcName, funcType, srcPath, varTypeBegin)   scanCallFunctionFromNode(cur, funcName, funcType, srcPath, varTypeBegin, true)

static void scanCallFunctionFromNode(xmlNodePtr cur, char *funcName, char *funcType, char *srcPath, varType *varTypeBegin, bool flag);

bool ExtractFuncFromCPPXML(char *docName)
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
            char *funcType = "extern";
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"template"))
                    break;
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"specifier"))
                {
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), "static") == 0)
                        funcType = "static";
                    else if(strcasecmp((char*)xmlNodeGetContent(temp_cur), "inline") == 0)
                        break;
                }
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    memset(src_dir, 0, DIRPATH_MAX);
                    //删除开头的temp_和结尾的.xml
                    strncpy(src_dir, (char *)&(docName[5]), strlen(docName)-9);
                    xmlChar* attr_value;
                    if(temp_cur->children->last != NULL)
                    {
                        if(xmlStrcmp(temp_cur->children->last->name, (const xmlChar*)"position"))
                            attr_value = xmlGetProp(temp_cur->children->last, (xmlChar*)"line");
                        else
                            attr_value = xmlGetProp(temp_cur->children, (xmlChar*)"line");
                          
                    }
                    else
                        attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");

                    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                    sprintf(sqlCommand, "insert into %s (funcName, type, sourceFile, line) value('%s', '%s', '%s', %s)", tempFuncScoreTableName,\
                        (char*)xmlNodeGetContent(temp_cur), funcType, src_dir, attr_value);
                        
                    if(!executeCommand(sqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                        RecordLog(error_info);
                    }
                    
                    varType *begin = ExtractVarTypeFromNode(cur, true);
                    varType *current = begin;
                    scanCallFunction(cur, (char*)xmlNodeGetContent(temp_cur), funcType, src_dir, begin);
                    while(current != NULL)
                    {
                        begin = begin->next;
                        //printf("%s(%d):%s\n", current->type, current->line, current->varName);
                        free(current);
                        current = begin;
                    }
                    
                    break;
                }
                temp_cur = temp_cur->next;
            }
        }
        else if(!xmlStrcmp(cur->name, (const xmlChar*)"class"))
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
                            memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                            sprintf(sqlCommand, "insert into %s (className, inheritType, inheritClassName) value('%s', '%s', '%s')", \
                            classInheritTableName, className, inheritType, inheritClassName);

                            mysql_real_query(mysqlConnect, sqlCommand, strlen(sqlCommand));
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
    
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "delete from %s where funcName not in (select calledFunc from %s) and type='static'", tempFuncScoreTableName, tempFuncCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    //merge tempFuncScore into funcScore
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "insert into %s select distinct * from %s", funcScoreTableName, tempFuncScoreTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    //clear tempFuncScore
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "truncate table %s", tempFuncScoreTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    // delete library function call record from funcCall
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "delete from %s where funcName not in (select funcName from %s) and funcCallType='static'", tempFuncCallTableName, tempFuncScoreTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    //merge tempFuncCall into funcCall
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "insert into %s select distinct * from %s", funcCallTableName, tempFuncCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    //clear tempFuncCall
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "truncate table %s", tempFuncCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
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
static void scanCallFunctionFromNode(xmlNodePtr cur, char *funcName, char *funcType, char *srcPath, varType *varTypeBegin, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                xmlChar* attr_value = NULL;
                char callFuncName[128] = {};
                if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                {
                    attr_value = xmlGetProp(cur->children->last, (xmlChar*)"line");
                }
                else
                {
                    attr_value = xmlGetProp(cur->children, (xmlChar*)"line");
                }
                if(!xmlStrcmp(cur->children->children->name, (const xmlChar*)"text"))
                    strcat(callFuncName, (char*)xmlNodeGetContent(cur->children));
                else
                {
                    if(strcasecmp((char*)xmlNodeGetContent(cur->children->last->prev), "::") != 0)
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
                        strcat(callFuncName, (char*)xmlNodeGetContent(cur->children->last));
                    }
                    else
                        strcat(callFuncName, (char*)xmlNodeGetContent(cur->children));
                }
                //删除递归调用
                if(strcasecmp(callFuncName, funcName) != 0)
                {
                    sprintf(sqlCommand, "insert into %s (funcName, funcCallType, sourceFile, calledFunc, CalledSrcFile, line, type) \
                        value('%s', '%s', '%s', '%s', '%s', %s, 'L')", tempFuncCallTableName, funcName, funcType, srcPath, callFuncName, srcPath, attr_value);
                    if(!executeCommand(sqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                        RecordLog(error_info);
                    }
                }
            }
        }
        
        scanCallFunctionFromNode(cur->children, funcName, funcType, srcPath, varTypeBegin, false);
        if(flag)
            break;
        cur = cur->next;
    }
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

//example static struct stu *stu1, *stu2, stu3;
//type = static struct
varType *ExtractVarTypeFromNode(xmlNodePtr cur, bool flag)
{
    varType *begin, *end, *current;
    begin = end = current = NULL;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"decl_stmt") || !xmlStrcmp(cur->name, (const xmlChar*)"parameter"))
        {
            if(begin == NULL)
                begin = end = malloc(sizeof(varType));
            else
                end = end->next = malloc(sizeof(varType));
            memset(end, 0, sizeof(varType));
            
            xmlNodePtr temp_cur = cur->children;
            xmlChar* attr_value = NULL;//xmlGetProp(cur->last, (xmlChar*)"line");
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
                            else if(!xmlStrcmp(temp->name, (const xmlChar*)"type"))
                            {
                                xmlNodePtr temp_type = temp->children;
                                while(temp_type != NULL)
                                {
                                    //handle struct stu
                                    if(!xmlStrcmp(temp_type->name, (const xmlChar*)"name"))
                                    {
                                        if(!xmlStrcmp(temp_type->children->name, (const xmlChar*)"text"))
                                        {
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
                                        break;
                                    }
                                    temp_type = temp_type->next;
                                }
                            }
                            //handle stu1
                            else if(!xmlStrcmp(temp->name, (const xmlChar*)"name"))
                            {
                                xmlNodePtr temp_name = temp->children;
                                if(!xmlStrcmp(temp_name->name, (const xmlChar*)"text"))
                                {
                                    strcat(end->varName, (char*)xmlNodeGetContent(temp));
                                    attr_value = xmlGetProp(temp, (xmlChar*)"line");
                                }
                                else
                                {
                                    strcat(end->varName, (char*)xmlNodeGetContent(temp_name));
                                    attr_value = xmlGetProp(temp_name, (xmlChar*)"line");
                                }
                                
                                end->line = StrToInt(attr_value);
                                strcat(end->type, type);
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
                        xmlNodePtr temp_name = temp_cur->children->next->children;
                        if(!xmlStrcmp(temp_name->name, (const xmlChar*)"text"))
                        {
                            strcat(end->varName, (char*)xmlNodeGetContent(temp_cur->children->next));
                            attr_value = xmlGetProp(temp_cur->children->next, (xmlChar*)"line");
                        }
                        else
                        {
                            strcat(end->varName, (char*)xmlNodeGetContent(temp_name));
                            attr_value = xmlGetProp(temp_name, (xmlChar*)"line");
                        }
                        end->line = StrToInt(attr_value);
                    }
                }
                temp_cur = temp_cur->next;
            }
        }
        else
        {
            current = ExtractVarTypeFromNode(cur->children, false);
            if(begin != NULL)
                end->next = current;
            else
                begin = end = current;
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