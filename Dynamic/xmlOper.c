/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : xmlOper.c
* Descripe     : parse xml file
******************************************************/

#include "xmlOper.h"

static int loopCount = 0;

void AddHeaderFile(xmlNodePtr root_node)
{
    xmlNodePtr cur = root_node->children;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar *) "include"))
        {
            xmlChar *value = xmlNodeGetContent(cur);
            char temp[1024] = "";
            sprintf(temp, "%s\n#include <sys/types.h>\n#include <sys/stat.h>\n#include <fcntl.h>\n#include <unistd.h>\n#include <string.h>", value);
            xmlNodeSetContent(cur, (xmlChar *)temp);
            
            break;
        }
        cur = cur->next;
    }
}

void AddLoopCount(xmlNodePtr funcBlockNode, char *funcName, char *srcPath)
{
    xmlNodePtr cur = funcBlockNode->children;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar *) "block") && (!xmlStrcmp(cur->parent->name, (const xmlChar *) "for") || \
            !xmlStrcmp(cur->parent->name, (const xmlChar *) "while") || !xmlStrcmp(cur->parent->name, (const xmlChar *) "do")))
        {
            loopCount++;
            xmlChar *blockType = xmlGetProp(cur, (xmlChar*)"type");
            char temp[1024] = "";
            xmlChar *value = xmlNodeGetContent(cur->parent->prev);
            sprintf(temp, "%sint count%d=0;", value, loopCount);
            xmlNodeSetContent(cur->parent->prev, (xmlChar *)temp);
            value = xmlNodeGetContent(cur->children);
            if(blockType != NULL && strcasecmp((char *)blockType, "pseudo") == 0)
            {
                //没有括号
                sprintf(temp, "{%scount%d++;}", value, loopCount);
            }
            else
            {
                sprintf(temp, "%scount%d++;", value, loopCount);
            }
            xmlNodeSetContent(cur->children, (xmlChar *)temp);
            
            value = xmlNodeGetContent(cur->parent->next);
            int temp_size = strlen(value);
            char *temp_str = malloc(sizeof(char)*(temp_size+1024));
            memset(temp_str, 0, sizeof(char)*(temp_size+1024));
            sprintf(temp_str, "char str_count%d[10]=\"\";\nsprintf(str_count%d, \"%%d\", count%d);\nint fd%d=open(\"./count_record.txt\", O_APPEND|O_RDWR|O_CREAT, 0644);\n \
            write(fd%d, \"srcPath=\", strlen(\"srcPath=\"));\n write(fd%d, \"%s\", strlen(\"%s\"));\nwrite(fd%d, \": funcName=\", strlen(\": funcName=\"));\n \
            write(fd%d, \"%s\", strlen(\"%s\"));\nwrite(fd%d, \": count%d=\", strlen(\": count%d=\"));\nwrite(fd%d, str_count%d, strlen(str_count%d));\nwrite(fd%d, \"\\n\", 1);\n \
            close(fd%d);%s", loopCount, loopCount, loopCount, loopCount, loopCount, loopCount, srcPath, srcPath, loopCount, loopCount, funcName, funcName, loopCount, \
            loopCount, loopCount, loopCount, loopCount, loopCount, loopCount, loopCount, value);
            xmlNodeSetContent(cur->parent->next, (xmlChar *)temp_str);
            free(temp_str);
            
            break;
        }
        
        AddLoopCount(cur, funcName, srcPath);
        cur = cur->next;
    }
}

SelfDefFuncNodeList *getFuncBlockNodeAndName(xmlNodePtr root_node)
{
    SelfDefFuncNodeList *begin = NULL;
    SelfDefFuncNodeList *end = NULL;

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
            while(temp_cur != NULL)
            {
                SelfDefFuncNode temp;
                if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"name"))
                {
                    if(strcasecmp("__attribute__", (char*)xmlNodeGetContent(temp_cur)) == 0)
                        break;
                    strcpy(temp.funcName, (char*)xmlNodeGetContent(temp_cur));
                }
                else if(!xmlStrcmp(temp_cur->name, (const xmlChar*)"block"))
                {
                    temp.blockNode = temp_cur;
                    if(begin == NULL)
                        begin = end = malloc(sizeof(SelfDefFuncNodeList));
                    else
                        end = end->next = malloc(sizeof(SelfDefFuncNodeList));
                    memset(end, 0, sizeof(SelfDefFuncNodeList));
                    
                    end->funcInfo = temp;
                    
                    break;
                }
                temp_cur = temp_cur->next;
            }
        }
        
        cur = cur->next;
    }
    
    return begin;
}

bool InsertCode(char *filePath)
{
    xmlDocPtr doc;
    xmlNodePtr cur;
    doc = xmlParseFile(filePath);
    if (doc == NULL)
    {
        fprintf(stderr, "Document not parsed successfully. \n");
        return false;
    }
    cur = xmlDocGetRootElement (doc);
    if (cur == NULL)
    {
        fprintf(stderr, "empty document\n");
        xmlFreeDoc(doc);
      return false;
    }
    //添加头文件
    AddHeaderFile(cur);
    
    SelfDefFuncNodeList *begin = NULL;
    SelfDefFuncNodeList *current = NULL;
    begin = current = getFuncBlockNodeAndName(cur);
    
    while(current != NULL)
    {
        loopCount = 0;
        AddLoopCount(current->funcInfo.blockNode, current->funcInfo.funcName, filePath);
        current = current->next;
    }
    
    current = begin;
    while(current != NULL)
    {
        begin = begin->next;
        free(current);
        current = begin;
    }
    
    xmlSaveFormatFile(filePath, doc, 0);
    xmlFreeDoc (doc);
    
    return true;
}