#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "config.h"
#include "sourceOper.h"

int main (int argc, char **argv)
{
    /*
    char *docname;
    docname = "test.c.xml";
    InsertCode(docname);
    */
    
    getSoftWareConfInfo();
    getProgramName(srcPath);
    /*
    BuildSrcToXml();
    BuildInsertXml();
    BuildXmlToSrc();
     * */
    ClearTmp();
    
    return 0;
}
