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
        if(xmlStrcmp(cur->name, (const xmlChar *) "comment"))
        {
            xmlChar *value = xmlNodeGetContent(cur);
            char temp[1024] = "";
            //sprintf(temp, "#include <sys/types.h>\n#include <sys/stat.h>\n#include <fcntl.h>\n#include <unistd.h>\n#include <string.h>\n%s", value);
            sprintf(temp, "#include <insertFile.h>\n%s", value);
            xmlNodeSetContent(cur, (xmlChar *)temp);
            break;
        }
        
        cur = cur->next;
    }
}

void AddLoopCount(xmlNodePtr funcBlockNode, char *funcName, char *srcPath, int *loopCount)
{
    xmlNodePtr cur = funcBlockNode->children;
    while(cur != NULL)
    {
        AddLoopCount(cur, funcName, srcPath, loopCount);
        
        if(!xmlStrcmp(cur->name, (const xmlChar *) "block") && (!xmlStrcmp(cur->parent->name, (const xmlChar *) "for") || \
            !xmlStrcmp(cur->parent->name, (const xmlChar *) "while") || !xmlStrcmp(cur->parent->name, (const xmlChar *) "do")))
        {
            (*loopCount)++;
            char temp[1024] = "";
            xmlChar *value = NULL;
            
            xmlChar *blockType = xmlGetProp(cur, (xmlChar*)"type");
            if(blockType != NULL && strcasecmp((char *)blockType, "pseudo") == 0)
            {
                //没有括号
                memset(temp, 0, 1024);
                sprintf(temp, "\ncount%d++;}", *loopCount);
                xmlNewTextChild(cur, NULL, (const xmlChar*)"keyword", (xmlChar *)temp);
                if(!xmlStrcmp(cur->parent->name, (const xmlChar *) "do"))
                {
                    value = xmlNodeGetContent(cur->prev);
                    memset(temp, 0, 1024);
                    sprintf(temp, "%s\n{", value);
                    xmlNodeSetContent(cur->prev, (xmlChar *)temp);
                }
                else
                {
                    xmlNodePtr temp_cur = cur->prev;
                    while(!xmlStrcmp(temp_cur->name, (const xmlChar *) "text"))
                        temp_cur = temp_cur->prev;
                    xmlNewTextChild(temp_cur, NULL, (const xmlChar*)"keyword", (xmlChar *)"\n{");
                }
            }
            else
            {
                xmlNodePtr temp_cur = cur->last->prev;
                //处理空的循环体
                if(temp_cur == NULL)
                {
                    //空的循环不做任何插装
                    goto next;
                }
                while(!xmlStrcmp(temp_cur->name, (const xmlChar *) "text"))
                    temp_cur = temp_cur->prev;
                memset(temp, 0, 1024);
                sprintf(temp, "\ncount%d++;", *loopCount);
                xmlNewTextChild(temp_cur, NULL, (const xmlChar*)"keyword", (xmlChar *)temp);
            }
            
            if(cur->parent->prev != NULL)
            {
                value = xmlNodeGetContent(cur->parent->prev);
                sprintf(temp, "%s\n{int count%d=0;", value, *loopCount);
                xmlNodeSetContent(cur->parent->prev, (xmlChar *)temp);
            }
            else
            {
                xmlNodePtr temp_prev_node = cur->parent;
                while(temp_prev_node->prev == NULL || !xmlStrcmp(temp_prev_node->prev->name, (const xmlChar *) "text"))
                {
                    if(temp_prev_node->prev == NULL)
                        temp_prev_node = temp_prev_node->parent;
                    else
                        temp_prev_node = temp_prev_node->prev;
                }
                
                xmlNodePtr temp_then = temp_prev_node->prev;
                while(temp_then != NULL)
                {
                    if(!xmlStrcmp(temp_then->name, (const xmlChar *)"text") || !xmlStrcmp(temp_then->name, (const xmlChar *)"comment"))
                        temp_then = temp_then->prev;
                    else
                        break;
                }
                if(temp_then != NULL && !xmlStrcmp(temp_then->name, (const xmlChar *)"then"))
                {
                    //处理mysql dota.c(1095行)
                    //处理else 后紧跟着循环情况
                    value = xmlNodeGetContent(temp_prev_node->next->children);
                    sprintf(temp, "%s\n{int count%d=0;", value, *loopCount);
                    xmlNodeSetContent(temp_prev_node->next->children, (xmlChar *)temp);
                }
                else
                {
                    memset(temp, 0, 1024);
                    sprintf(temp, "\n{int count%d=0;", *loopCount);
                    xmlNewTextChild(temp_prev_node->prev, NULL, (const xmlChar*)"keyword", (xmlChar *)temp);
                }
            }
            
            int position = ExtractLastCharIndex(srcPath, '/');
            char temp_record[128] = "";
            sprintf(temp_record, "%s.txt", &(srcPath[position+1]));
            
            xmlNodePtr temp_next_node = cur->parent;
            while(temp_next_node == NULL || !xmlStrcmp(temp_next_node->name, (const xmlChar *) "text"))
            {
                if(temp_next_node == NULL)
                    temp_next_node = temp_next_node->parent;
                else
                    temp_next_node = temp_next_node->next;
            }
            
            char *temp_str = malloc(sizeof(char)*(1024));
            memset(temp_str, 0, sizeof(char)*(+1024));
            /*
            sprintf(temp_str, "\nchar str_count%d[10]=\"\";\nsprintf(str_count%d, \"%%d\", count%d);\nint fd%d=open(\"./record/%s.txt\", O_APPEND|O_RDWR|O_CREAT, 0644);\n \
            write(fd%d, \"srcPath=\", 8);\n write(fd%d, \"%s\", strnlen(\"%s\", 1024));\nwrite(fd%d, \": funcName=\", 11);\n \
            write(fd%d, \"%s\", strnlen(\"%s\", 128));\nwrite(fd%d, \": count%d=\", strnlen(\": count%d=\",15));\nwrite(fd%d, str_count%d, strnlen(str_count%d, 15));\nwrite(fd%d, \"\\n\", 1);\n \
            close(fd%d);}", *loopCount, *loopCount, *loopCount, *loopCount, temp_record, *loopCount, *loopCount, srcPath, srcPath, *loopCount, *loopCount, funcName, \
            funcName, *loopCount, *loopCount, *loopCount, *loopCount, *loopCount, *loopCount, *loopCount, *loopCount);
            */
            
            sprintf(temp_str, "\ninsert_count((char *)\"%s\", (char *)\"%s\", (char *)\"%s\", %d, count%d);}", temp_record, srcPath, funcName, *loopCount, *loopCount);
            xmlNewTextChild(temp_next_node, NULL, (const xmlChar*)"keyword", (xmlChar *)temp_str);
            free(temp_str);
        }
        
next:
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
                    removeChar(temp.funcName, '\n');
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
    
    char src_dirPath[DIRPATH_MAX] = "";
    strncpy(src_dirPath, &(filePath[5]), strlen(filePath)-9);
    if(strstr(src_dirPath, "resolveip.c") != NULL)
    {
        //srcml 无法正确解析resolveip.c
        xmlFreeDoc (doc);
    }
    else
    {
        //添加头文件
        AddHeaderFile(cur);
        
        SelfDefFuncNodeList *begin = NULL;
        SelfDefFuncNodeList *current = NULL;
        begin = current = getFuncBlockNodeAndName(cur);
        
        while(current != NULL)
        {
            int loopCount = 0;
            AddLoopCount(current->funcInfo.blockNode, current->funcInfo.funcName, src_dirPath, &loopCount);
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
    }
    
    return true;
}