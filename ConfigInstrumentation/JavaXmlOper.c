/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : JavaXmlOper.h
* Descripe     : parse Java language xml file
******************************************************/

#include "JavaXmlOper.h"

void AddJavaPackage(xmlNodePtr root_node)
{
    xmlNodePtr cur = root_node->children;
    //查看是否以及添加过
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar *) "import"))
        {
            if(strcmp((char*)xmlNodeGetContent(cur), "\nimport java.io.IO;\n") == 0)
                return ;
        }
        
        cur = cur->next;
    }
    
    cur = root_node->children;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar *) "import"))
        {
            char temp[1024] = "";
            //sprintf(temp, "#include <sys/types.h>\n#include <sys/stat.h>\n#include <fcntl.h>\n#include <unistd.h>\n#include <string.h>\n%s", value);
            sprintf(temp, "\nimport java.io.IO;\n");
            xmlNodePtr newNode = xmlNewNode(NULL, (const xmlChar*)"import");
            xmlNodePtr newText = xmlNewText((xmlChar *)temp);
            xmlAddChild(newNode, newText);
            newNode->next = cur;
            newNode->prev = cur->prev;
            cur->prev = newNode;
            newNode->prev->next = newNode;
            //xmlNewTextChild(cur, NULL, (const xmlChar*)"keyword", (xmlChar *)temp);
            break;
        }
        
        cur = cur->next;
    }
}

xmlNodePtr getJavaFuncBlockNodeByFuncName(xmlNodePtr root_node, char *funcName)
{
    xmlNodePtr cur = root_node->children;
    while(cur != NULL)
    {
        xmlNodePtr ret_Node = getJavaFuncBlockNodeByFuncName(cur, funcName);
        if(ret_Node != NULL)
        {
            //递归结束，定位到要查找的函数节点
            return ret_Node;
        }
        
        if(!xmlStrcmp(cur->name, (const xmlChar*)"function") || !xmlStrcmp(cur->name, (const xmlChar*)"constructor"))
        {
            xmlNodePtr temp_cur = cur->children;
            char tmp_funcName[MAX_FUNCTION_NAME_NUM];
            while(temp_cur != NULL)
            {
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
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

void AddJavaMarkerCode(xmlNodePtr funcBlockNode, char *confName)
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