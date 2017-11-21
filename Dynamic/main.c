#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

void parseStory (xmlNodePtr cur, char *keyword)
{
while (cur != NULL)
  {
      if((!xmlStrcmp (cur->name, (const xmlChar *) "block")) && (!xmlStrcmp (cur->parent->name, (const xmlChar *) "for")))
      {
          xmlChar *value = xmlNodeGetContent(cur->parent->prev); 
          char temp[1024] = "";
          
          sprintf(temp, "%sint count;", value);
          xmlNodeSetContent(cur->parent->prev, temp);
          value = xmlNodeGetContent(cur->children);
          sprintf(temp, "{%scount++;", value);
          xmlNodeSetContent(cur->children, temp);
          xmlNewChild(cur, NULL, BAD_CAST "node1",BAD_CAST "}");  
      }
      parseStory(cur->children, keyword);
      cur = cur->next;
  }
   
   return;
}

xmlDocPtr parseDoc (char *docname, char *keyword)
{
  xmlDocPtr doc;
  xmlNodePtr cur;
  doc = xmlParseFile (docname);
  //doc = xmlReadFile(docname, NULL, XML_PARSE_NOBLANKS); //读取xml文件时忽略空格
  if (doc == NULL)
  {
      fprintf (stderr, "Document not parsed successfully. \n");
      return (NULL);
  }
  cur = xmlDocGetRootElement (doc);
  if (cur == NULL)
  {
      fprintf (stderr, "empty document\n");
      xmlFreeDoc (doc);
      return (NULL);
  }
  if (xmlStrcmp (cur->name, (const xmlChar *) "unit"))
  {
      fprintf (stderr, "document of the wrong type, root node != story\n");
      xmlFreeDoc (doc);
      return (NULL);
  }
  cur = cur->xmlChildrenNode;
    parseStory(cur, keyword);
  return (doc);
}

int main (int argc, char **argv)
{
  char *docname;
  char *keyword;
  xmlDocPtr doc;
  docname = "test.c.xml";
  keyword = "int";
  doc = parseDoc (docname, keyword);
  if (doc != NULL)
  {
      xmlSaveFormatFile (docname, doc, 0);
      //xmlSaveFormatFile (docname, doc, 1);
      xmlFreeDoc (doc);
  }
  return (1);
}
