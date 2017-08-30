#include "buildFuncLibrary.h"

static char error_info[200];
static char lineData[LINE_CHAR_MAX_NUM];
static char subStr2[2][MAX_SUBSTR];
static char insertCommand[LINE_CHAR_MAX_NUM];

bool buildLibrary()
{
    bool ret = false;

    //create funcLibrary table
    if(executeCommand(createFuncLibraryTable))
        ret = true;
    else
    {
        RecordLog("create funcLibrary table failure!\n");
        ret = false;
    }
    
    int fd = OpenFile(FUNCLIBRARY_PATH, O_RDONLY);
    if(fd == -1)
    {
        memset(error_info, 0, 200);
        sprintf(error_info, "open file(%s) failed: %s\n", FUNCLIBRARY_PATH, strerror(errno));
		RecordLog(error_info);
        
        return ret;
    }
    memset(lineData, 0, LINE_CHAR_MAX_NUM);
    while(ReadLine(fd, lineData) == -1)
    {
        removeChar(lineData, ' ');
        cutStrByLabel(lineData, '/', subStr2, 2);
        memset(insertCommand, 0, LINE_CHAR_MAX_NUM);
        sprintf(insertCommand, "insert into funcLibrary value('%s', '%s')", subStr2[0], subStr2[1]);
        if(executeCommand(insertCommand))
            ret = true;
        else
        {
            RecordLog("insert function library failure!\n");
            ret = false;
        }     
        memset(lineData, 0, LINE_CHAR_MAX_NUM);
    }
    CloseFile(fd);
    
    return ret;
}