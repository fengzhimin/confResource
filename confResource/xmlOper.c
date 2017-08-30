#include "xmlOper.h"

static char error_info[200];
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
        memset(error_info, 0, 200);
        sprintf(error_info, "Document(%s) not parsed successfully. \n", docName);
		RecordLog(error_info);
        return false;
    }
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        memset(error_info, 0, 200);
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
                    xmlChar* attr_value = xmlGetProp(temp_cur, (xmlChar*)"line");
                    //printf("%s: %s\t", (char*)xmlNodeGetContent(temp_cur), attr_value);
                    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                    memset(src_dir, 0, DIRPATH_MAX);
                    //删除开头的temp_和结尾的.xml
                    strncpy(src_dir, (char *)&(docName[5]), strlen(docName)-9);
                    sprintf(sqlCommand, "insert into funcScore (funcName, sourceFile, line) value('%s', '%s', %s)", \
                        (char*)xmlNodeGetContent(temp_cur), src_dir, attr_value);
                    if(!executeCommand(sqlCommand))
                    {
                        memset(error_info, 0, 200);
                        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                        RecordLog(error_info);
                    }
                    scanCallFunction(cur->children, (char*)xmlNodeGetContent(temp_cur));
                    //printf("\n");
                }
                temp_cur = temp_cur->next;
            }
            
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);  
    return true;  
}

void scanCallFunction(xmlNodePtr cur, char *funcName)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
        {
            if(cur->children->last != NULL)
            {
                memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
                xmlChar* attr_value = NULL;
                if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                {
                    attr_value = xmlGetProp(cur->children->last, (xmlChar*)"line");
                    //printf("%s(%s)\t", (char*)xmlNodeGetContent(cur->children->last), attr_value);
                    sprintf(sqlCommand, "insert into funcCall value('%s', '%s', %s, 'L')", funcName, (char*)xmlNodeGetContent(cur->children->last), attr_value);
                }
                else
                {
                    attr_value = xmlGetProp(cur->children, (xmlChar*)"line");
                    //printf("%s(%s)\t", (char*)xmlNodeGetContent(cur->children), attr_value);
                    sprintf(sqlCommand, "insert into funcCall value('%s', '%s', %s, 'L')", funcName, (char*)xmlNodeGetContent(cur->children), attr_value);
                }
                
                if(!executeCommand(sqlCommand))
                {
                    memset(error_info, 0, 200);
                    sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
                    RecordLog(error_info);
                }
            }
        }
        else
            scanCallFunction(cur->children, funcName);
        cur = cur->next;
    }
}