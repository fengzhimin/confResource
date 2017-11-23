/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : xmlOper.c
* Descripe     : parse xml file
******************************************************/

#include "xmlOper.h"

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

void AddLoopCount(xmlNodePtr funcBlockNode, char *funcName, char *srcPath, int loopCount)
{
    xmlNodePtr cur = funcBlockNode->children;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar *) "block") && (!xmlStrcmp(cur->parent->name, (const xmlChar *) "for") || \
            !xmlStrcmp(cur->parent->name, (const xmlChar *) "while") || !xmlStrcmp(cur->parent->name, (const xmlChar *) "do")))
        {
            loopCount++;
            char temp[1024] = "";
            xmlChar *value = NULL;
            
            xmlChar *blockType = xmlGetProp(cur, (xmlChar*)"type");
            if(blockType != NULL && strcasecmp((char *)blockType, "pseudo") == 0)
            {
                //没有括号
                sprintf(temp, "count%d++;}", loopCount);
                xmlNewTextChild(cur, NULL, (const xmlChar*)"keyword", (xmlChar *)temp);
                xmlNodePtr temp_cur = cur->prev;
                while(!xmlStrcmp(temp_cur->name, (const xmlChar *) "text"))
                    temp_cur = temp_cur->prev;
                xmlNewTextChild(temp_cur, NULL, (const xmlChar*)"keyword", (xmlChar *)"{");
            }
            else
            {
                xmlNodePtr temp_cur = cur->last->prev;
                while(!xmlStrcmp(temp_cur->name, (const xmlChar *) "text"))
                    temp_cur = temp_cur->prev;
                sprintf(temp, "count%d++;", loopCount);
                xmlNewTextChild(temp_cur, NULL, (const xmlChar*)"keyword", (xmlChar *)temp);
            }
            
            if(cur->parent->prev != NULL)
            {
                value = xmlNodeGetContent(cur->parent->prev);
                sprintf(temp, "%s{int count%d=0;", value, loopCount);
                xmlNodeSetContent(cur->parent->prev, (xmlChar *)temp);
            }
            else
            {
                value = xmlNodeGetContent(cur->parent);
                sprintf(temp, "{int count%d=0;%s", loopCount, value);
                xmlNodeSetContent(cur->parent, (xmlChar *)temp);
            }
            
            int position = ExtractLastCharIndex(srcPath, '/');
            char temp_record[128] = "";
            strcpy(temp_record, &(srcPath[position+1]));
            
            if(cur->parent->next != NULL)
            {
                value = xmlNodeGetContent(cur->parent->next);
                int temp_size = strlen((char *)value);
                char *temp_str = malloc(sizeof(char)*(temp_size+1024));
                memset(temp_str, 0, sizeof(char)*(temp_size+1024));
                sprintf(temp_str, "char str_count%d[10]=\"\";\nsprintf(str_count%d, \"%%d\", count%d);\nint fd%d=open(\"./record/%s.txt\", O_APPEND|O_RDWR|O_CREAT, 0644);\n \
                write(fd%d, \"srcPath=\", strlen(\"srcPath=\"));\n write(fd%d, \"%s\", strlen(\"%s\"));\nwrite(fd%d, \": funcName=\", strlen(\": funcName=\"));\n \
                write(fd%d, \"%s\", strlen(\"%s\"));\nwrite(fd%d, \": count%d=\", strlen(\": count%d=\"));\nwrite(fd%d, str_count%d, strlen(str_count%d));\nwrite(fd%d, \"\\n\", 1);\n \
                close(fd%d);}%s", loopCount, loopCount, loopCount, loopCount, temp_record, loopCount, loopCount, srcPath, srcPath, loopCount, loopCount, funcName, funcName, loopCount, \
                loopCount, loopCount, loopCount, loopCount, loopCount, loopCount, loopCount, value);
                xmlNodeSetContent(cur->parent->next, (xmlChar *)temp_str);
                free(temp_str);
            }
            else
            {
                value = xmlNodeGetContent(cur->parent);
                int temp_size = strlen((char *)value);
                char *temp_str = malloc(sizeof(char)*(temp_size+1024));
                memset(temp_str, 0, sizeof(char)*(temp_size+1024));
                sprintf(temp_str, "%schar str_count%d[10]=\"\";\nsprintf(str_count%d, \"%%d\", count%d);\nint fd%d=open(\"./record/%s.txt\", O_APPEND|O_RDWR|O_CREAT, 0644);\n \
                write(fd%d, \"srcPath=\", strlen(\"srcPath=\"));\n write(fd%d, \"%s\", strlen(\"%s\"));\nwrite(fd%d, \": funcName=\", strlen(\": funcName=\"));\n \
                write(fd%d, \"%s\", strlen(\"%s\"));\nwrite(fd%d, \": count%d=\", strlen(\": count%d=\"));\nwrite(fd%d, str_count%d, strlen(str_count%d));\nwrite(fd%d, \"\\n\", 1);\n \
                close(fd%d);}", value, loopCount, loopCount, loopCount, loopCount, temp_record, loopCount, loopCount, srcPath, srcPath, loopCount, loopCount, funcName, funcName, loopCount, \
                loopCount, loopCount, loopCount, loopCount, loopCount, loopCount, loopCount);
                xmlNodeSetContent(cur->parent, (xmlChar *)temp_str);
                free(temp_str);
            }
            
            break;
        }
        
        AddLoopCount(cur, funcName, srcPath, loopCount);
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
    char src_dirPath[DIRPATH_MAX] = "";
    strncpy(src_dirPath, &(filePath[5]), strlen(filePath)-9);
    while(current != NULL)
    {
        AddLoopCount(current->funcInfo.blockNode, current->funcInfo.funcName, src_dirPath, 0);
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