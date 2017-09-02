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
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    memset(src_dir, 0, DIRPATH_MAX);
                    //删除开头的temp_和结尾的.xml
                    strncpy(src_dir, (char *)&(docName[5]), strlen(docName)-9);
                    xmlChar* attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                    sprintf(sqlCommand, "insert into funcScore (funcName, sourceFile, line) value('%s', '%s', %s)", \
                        (char*)xmlNodeGetContent(temp_cur), src_dir, attr_value);
                    if(!executeCommand(sqlCommand))
                    {
                        memset(error_info, 0, LOGINFO_LENGTH);
                        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                        RecordLog(error_info);
                    }
                    scanCallFunction(cur->children, (char*)xmlNodeGetContent(temp_cur), src_dir);
                    
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

void scanCallFunction(xmlNodePtr cur, char *funcName, char *srcPath)
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
                sprintf(sqlCommand, "insert into funcCall value('%s', '%s', '%s', %s, 'L')", funcName, srcPath, \
                    callFuncName, attr_value);
                if(!executeCommand(sqlCommand))
                {
                    memset(error_info, 0, LOGINFO_LENGTH);
                    sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                    RecordLog(error_info);
                }
            }
        }
        else
            scanCallFunction(cur->children, funcName, srcPath);
        cur = cur->next;
    }
}

funcList *ExtractVarUsedPos(char *varName, char *xmlFilePath)
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
            while(temp_cur != NULL)
            {
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
            if(scanVarIsUsed(temp_cur, varName))
            {
                if(begin == NULL)
                    begin = end = malloc(sizeof(funcList));
                else
                    end = end->next = malloc(sizeof(funcList));
                memset(end, 0, sizeof(funcList));
                if(strlen((char*)xmlNodeGetContent(temp_cur)) < MAX_FUNCNAME_LENGTH)
                {
                    strcpy(end->funcName, (char*)xmlNodeGetContent(temp_cur));
                    end->line = StrToInt((char *)attr_value);
                }
                else
                {
                    memset(src_dir, 0, DIRPATH_MAX);
                    //删除开头的temp_和结尾的.xml
                    strncpy(src_dir, (char *)&(xmlFilePath[5]), strlen(xmlFilePath)-9);
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

bool scanVarIsUsed(xmlNodePtr cur, char *varName)
{
    bool ret = false;
    while(cur != NULL)
    {
        if(cur->children != NULL)
        {
            if(cur->children->next != NULL)
            {
                if(!xmlStrcmp(cur->children->name, (const xmlChar*)"text") && !xmlStrcmp(cur->children->next->name, (const xmlChar*)"position"))
                {
                    char *str = (char*)xmlNodeGetContent(cur);
                    //xmlChar* attr_value = xmlGetProp(cur, (xmlChar*)"type");
                    if(strcasecmp((char *)(cur->name), "literal") != 0)
                    {
                        //attr_value = xmlGetProp(cur, (xmlChar*)"line");
                        if(strcasecmp(str, varName) == 0)
                        {
                            //printf("%s(%s) ", str, attr_value);
                            ret = true;
                        }
                    }
                }
                else
                    ret |= scanVarIsUsed(cur->children, varName);
            }
            else
                ret |= scanVarIsUsed(cur->children, varName);
        }
        
        cur = cur->next;
    }
    
    return ret;
}
