#ifndef __XMLOPER_H__
#define __XMLOPER_H__

#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <libxml/xmlmemory.h>  
#include <libxml/parser.h>

void parseDoc(char *docname);

void parseStory(xmlDocPtr doc, xmlNodePtr cur);

#endif