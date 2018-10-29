/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : xmlOper.c
* Descripe     : parse xml file
******************************************************/

#include "xmlOper.h"

static char error_info[LOGINFO_LENGTH];

bool InsertMarkerCode(HandledConf handledConfig)
{
    InstrumentInfo *instrumentInfo = handledConfig.insertInfo;
    int instrumentInfoSize = handledConfig.insertInfoSize;
    for(int index = 0; index < instrumentInfoSize; index++)
    {
        xmlDocPtr doc;
        xmlNodePtr cur;
        char src_dirPath[MAX_PATH_LENGTH] = "";
        sprintf(src_dirPath, "temp_%s/%s.xml", programName, instrumentInfo[index].srcPath);
        doc = xmlParseFile(src_dirPath);
        if (doc == NULL)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "Document not parsed successfully! srcPath('%s') funcName('%s').\n", \
                src_dirPath, instrumentInfo[index].funcName);
            Error(error_info);
            return false;
        }
        cur = xmlDocGetRootElement (doc);
        if (cur == NULL)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "Empty document! srcPath('%s') funcName('%s').\n", \
                src_dirPath, instrumentInfo[index].funcName);
            Error(error_info);
            xmlFreeDoc(doc);
          return false;
        }
        
        xmlNodePtr funcBlock = NULL;
        //添加头文件/或Java包
        int fileType = judgeXmlFileType(src_dirPath);
        switch(fileType)
        {
        //handle C language program
        case 1:
            AddCHeaderFile(cur);
            funcBlock = getCFuncBlockNodeByFuncName(cur, instrumentInfo[index].funcName);
            if(funcBlock != NULL)
            {
                AddCMarkerCode(funcBlock, handledConfig.confName);
            }
            break;
        //handle C++ language program
        case 2:
            AddCPPHeaderFile(cur);
            funcBlock = getCPPFuncBlockNodeByFuncName(cur, instrumentInfo[index].funcName);
            if(funcBlock != NULL)
            {
                AddCPPMarkerCode(funcBlock, handledConfig.confName);
            }
            break;
        //handle Java language program
        case 3:
            AddJavaPackage(cur);
            funcBlock = getJavaFuncBlockNodeByFuncName(cur, instrumentInfo[index].funcName);
            if(funcBlock != NULL)
            {
                AddJavaMarkerCode(funcBlock, handledConfig.confName);
            }
            break;
        default:
            break;
        }
        
        if(funcBlock == NULL)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "get function block failure! srcPath('%s') funcName('%s').\n", \
                src_dirPath, instrumentInfo[index].funcName);
            Error(error_info);
        }
        xmlSaveFormatFile(src_dirPath, doc, 0);
        xmlFreeDoc (doc);
    }
    
    return true;
}