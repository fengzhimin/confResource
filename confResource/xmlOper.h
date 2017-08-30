#ifndef __XMLOPER_H__
#define __XMLOPER_H__

#include <stdio.h> 
#include <string.h>  
#include <stdlib.h>
#include <stdbool.h>
#include <libxml/xmlmemory.h>  
#include <libxml/parser.h>
#include "logOper.h"

/*******************************
 * func: Extract function Name from XML
 * return: true = success    false = failure
 * @para docName: xml path
********************************/
bool ExtractFuncFromXML(char *docName);

/*******************************
 * func: scan call function
 * return: void
 * @para cur: current Node
 * @para funcName: current self-define function name
********************************/
void scanCallFunction(xmlNodePtr cur, char *funcName);

#endif