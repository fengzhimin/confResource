/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CPPXmlOper.h
* Descripe     : parse C++ language xml file
******************************************************/

#include "CPPXmlOper.h"

void AddCPPHeaderFile(xmlNodePtr root_node)
{
    xmlNodePtr cur = root_node->children;
    //查看是否以及添加过
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar *) "include"))
        {
            if(strcmp((char*)xmlNodeGetContent(cur), "\n#include <insertFile.h>\n") == 0)
                return ;
        }
        
        cur = cur->next;
    }
    
    cur = root_node->children;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar *) "include"))
        {
            char temp[1024] = "";
            //sprintf(temp, "#include <sys/types.h>\n#include <sys/stat.h>\n#include <fcntl.h>\n#include <unistd.h>\n#include <string.h>\n%s", value);
            sprintf(temp, "\n#include <insertFile.h>\n");
            xmlNodePtr newNode = xmlNewNode(NULL, (const xmlChar*)"include");
            xmlNodePtr newText = xmlNewText((xmlChar *)temp);
            xmlAddChild(newNode, newText);
            newNode->next = cur;
            newNode->prev = cur->prev;
            cur->prev = newNode;
            newNode->prev->next = newNode;
            break;
        }
        
        cur = cur->next;
    }
}

xmlNodePtr getCPPFuncBlockNodeByFuncName(xmlNodePtr root_node, char *funcName)
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

void AddCPPMarkerCode(xmlNodePtr funcBlockNode, char *confName)
{
    xmlNodePtr cur = funcBlockNode->children;
    char temp_str[1024] = "";
    sprintf(temp_str, "insert_count((char *)\"%s\");\n", confName);
    while(cur != NULL)
    {
        if(xmlStrcmp(cur->name, (const xmlChar *) "text"))
        {
            xmlNodePtr newNode = xmlNewNode(NULL, (const xmlChar*)"keyword");
            xmlNodePtr newText = xmlNewText((xmlChar *)temp_str);
            xmlAddChild(newNode, newText);
            newNode->next = cur;
            newNode->prev = cur->prev;
            cur->prev = newNode;
            newNode->prev->next = newNode;
            newNode->parent = funcBlockNode;
        }

        cur = cur->next;
    }
}