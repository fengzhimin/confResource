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

#define scanCallFunction(cur, funcName, funcType, srcPath)   scanCallFunctionFromNode(cur, funcName, funcType, srcPath, true)

static void scanCallFunctionFromNode(xmlNodePtr cur, char *funcName, char *funcType, char *srcPath, bool flag);

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
            bool funcType = false;
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"template"))
                    break;
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"specifier"))
                {
                    if(strcasecmp((char*)xmlNodeGetContent(temp_cur), "static") == 0)
                        funcType = true;
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
    
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "delete from funcScore where funcName not in (select calledFunc from funcCall) and type='static'");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "delete from funcCall where funcName not in (select funcName from funcScore)");
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
********************************/
static void scanCallFunctionFromNode(xmlNodePtr cur, char *funcName, char *funcType, char *srcPath, bool flag)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(strcasecmp(funcName, "Item_func_curdate::fix_length_and_dec") == 0)
                printf("debug");
            if(cur->children->last != NULL)
            {
                memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                xmlChar* attr_value = NULL;
                char *callFuncName = NULL;
                if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                {
                    attr_value = xmlGetProp(cur->children->last, (xmlChar*)"line");
                }
                else
                {
                    attr_value = xmlGetProp(cur->children, (xmlChar*)"line");
                }
                callFuncName = (char*)xmlNodeGetContent(cur->children);
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