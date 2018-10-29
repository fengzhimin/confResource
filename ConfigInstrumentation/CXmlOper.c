/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CXmlOper.h
* Descripe     : parse C language xml file
******************************************************/

#include "CXmlOper.h"

void AddCHeaderFile(xmlNodePtr root_node)
{
    xmlNodePtr cur = root_node->children;
    while(cur != NULL)
    {
        if(xmlStrcmp(cur->name, (const xmlChar *) "text"))
        {
            char temp[1024] = "";
            //sprintf(temp, "#include <sys/types.h>\n#include <sys/stat.h>\n#include <fcntl.h>\n#include <unistd.h>\n#include <string.h>\n%s", value);
            sprintf(temp, "\n#include <insertFile.h>");
            xmlNewTextChild(cur, NULL, (const xmlChar*)"keyword", (xmlChar *)temp);
            break;
        }
        
        cur = cur->next;
    }
}

xmlNodePtr getCFuncBlockNodeByFuncName(xmlNodePtr root_node, char *funcName)
{
    xmlNodePtr cur = root_node->children;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || \
            (!xmlStrcmp(cur->name, (const xmlChar*)"extern") && cur->children != NULL && !xmlStrcmp(cur->last->name, (const xmlChar*)"function")))
        {
            xmlNodePtr funcNode;
            if(!xmlStrcmp(cur->name, (const xmlChar*)"function"))
                funcNode = cur;
            else
                funcNode = cur->last;
            xmlNodePtr temp_cur = funcNode->children;
            char tmp_funcName[MAX_FUNCTION_NAME_NUM];
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(strcasecmp("__attribute__", (char*)xmlNodeGetContent(temp_cur)) == 0)
                        break;
                    strcpy(tmp_funcName, (char*)xmlNodeGetContent(temp_cur));
                    removeChar(tmp_funcName, '\n');
                    if(strcmp(tmp_funcName, funcName) != 0)
                        break;
                }
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"block") && strcmp(tmp_funcName, funcName) == 0)
                {
                    return temp_cur;
                }
                temp_cur = temp_cur->next;
            }
        }
        
        cur = cur->next;
    }
    
    return NULL;
}

void AddCMarkerCode(xmlNodePtr funcBlockNode, char *confName)
{
    xmlNodePtr cur = funcBlockNode->children;
    char temp_str[1024] = "";
    while(cur != NULL)
    {
        if(xmlStrcmp(cur->name, (const xmlChar *) "text"))
        {
            sprintf(temp_str, "\ninsert_count((char *)\"%s\");", confName);
            xmlNewTextChild(cur, NULL, (const xmlChar*)"keyword", (xmlChar *)temp_str);
            break;
        }
        
        cur = cur->next;
    }
}