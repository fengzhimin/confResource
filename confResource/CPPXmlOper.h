/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : CPPXmlOper.h
* Descripe     : parse C Plus Plus language xml file
******************************************************/

#ifndef __CPPXMLOPER_H__
#define __CPPXMLOPER_H__

#include <stdio.h> 
#include <string.h>  
#include <stdlib.h>
#include <stdbool.h>
#include <libxml/xmlmemory.h>  
#include <libxml/parser.h>
#include "logOper.h"
#include "config.h"
#include "mysqlOper.h"

/*******************************
 * func: Extract function Name from C Plus Plus language XML file
 * return: true = success    false = failure
 * @para docName: xml path
********************************/
bool ExtractFuncFromCPPXML(char *docName);

#endif