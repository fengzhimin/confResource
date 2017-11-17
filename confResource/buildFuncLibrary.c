/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : buildFuncLibrary.c
* Descripe     : build library about relationship between function and system resource
******************************************************/

#include "buildFuncLibrary.h"

static char error_info[LOGINFO_LENGTH];
static char lineData[LINE_CHAR_MAX_NUM];
static char subStr3[3][MAX_SUBSTR];
static char insertCommand[LINE_CHAR_MAX_NUM];

bool buildLibrary()
{
    bool ret = false;
    
    //judge funcLibrary whether exist or not
    if(executeCommand(deleteFuncLibraryTable))
        ret = true;
    else
    {
        Error("delete funcLibrary table failure!\n");
        ret = false;
    }

    //create funcLibrary table
    if(executeCommand(createFuncLibraryTable))
        ret = true;
    else
    {
        Error("create funcLibrary table failure!\n");
        ret = false;
    }
    
    int fd = OpenFile(FUNCLIBRARY_PATH, O_RDONLY);
    if(fd == -1)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open file(%s) failed: %s\n", FUNCLIBRARY_PATH, strerror(errno));
		Error(error_info);
        
        return ret;
    }
    memset(lineData, 0, LINE_CHAR_MAX_NUM);
    while(ReadLine(fd, lineData) == -1)
    {
        removeChar(lineData, ' ');
        cutStrByLabel(lineData, '/', subStr3, 3);
        memset(insertCommand, 0, LINE_CHAR_MAX_NUM);
        sprintf(insertCommand, "insert into funcLibrary value('%s', '%s', %s)", subStr3[0], subStr3[1], subStr3[2]);
        if(executeCommand(insertCommand))
            ret = true;
        else
        {
            Error("insert function library failure!\n");
            ret = false;
        }     
        memset(lineData, 0, LINE_CHAR_MAX_NUM);
    }
    CloseFile(fd);
    
    return ret;
}
