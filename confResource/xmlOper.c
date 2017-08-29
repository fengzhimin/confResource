#include "xmlOper.h"

static char error_info[200];

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
            xmlChar* attr_value = xmlGetProp(cur->children->next, "line");
            printf("%s: %s\t", (char*)xmlNodeGetContent(cur->children->next), attr_value);
            scanCallFunction(cur->children);
            printf("\n");
        }
        cur = cur->next;
    }
      
    xmlFreeDoc(doc);  
    return true;  
}

void scanCallFunction(xmlNodePtr cur)
{
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)"call"))
            if(xmlStrcmp(cur->children->last->name, (const xmlChar*)"position"))
                printf("%s\t", (char*)xmlNodeGetContent(cur->children->last));
            else
                printf("%s\t", (char*)xmlNodeGetContent(cur->children));
        else
            scanCallFunction(cur->children);
        cur = cur->next;
    }
}