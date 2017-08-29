#include "xmlOper.h"

void parseStory (xmlDocPtr doc, xmlNodePtr cur) 
{
    while(cur != NULL)
    {
        if(strcasecmp(cur->children->name, "text") == 0)
        {
            printf("%s: %s\n", cur->name, (char*)xmlNodeGetContent(cur));  
        }
        else
        {
            parseStory(doc, cur->children);
        }
        cur = cur->next;
    }
    return;  
}  

void parseDoc(char *docname) {  
  
    xmlDocPtr doc;  
    xmlNodePtr cur;  
    xmlKeepBlanksDefault(0);
    doc = xmlParseFile(docname);  
      
    if (doc == NULL ) {  
        fprintf(stderr,"Document not parsed successfully. \n");  
        return;  
    }  
      
    cur = xmlDocGetRootElement(doc);  
      
    if (cur == NULL) {  
        fprintf(stderr,"empty document\n");  
        xmlFreeDoc(doc);  
        return;  
    }  
    
    cur = cur->children;
    while (cur != NULL) {
        parseStory (doc, cur);
        cur = cur->next;
    }  
      
    xmlFreeDoc(doc);  
    return;  
}  