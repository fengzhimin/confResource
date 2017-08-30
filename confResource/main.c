#include "mysqlOper.h"
#include "xmlOper.h"
#include "sourceOper.h"
#include "strOper.h"
#include "buildFuncLibrary.h"
#include "buildTempTable.h"

int main(int argc, char **argv) 
{
    if(!startMysql())
    {
        printf("connect mysql failure!\n");
        return -1;
    }
    //buildLibrary();
    buildTempTable();
    getProgramName("/home/fzm/Downloads/Program source code/httpd-2.4.27");
    convertProgram("/home/fzm/Downloads/Program source code/httpd-2.4.27");
    printf
    buildFuncScore();
    //deleteTempTable();
    
    stopMysql();
    
    return 0;
}