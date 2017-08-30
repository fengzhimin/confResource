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
    buildTempTable();
    getProgramName("/home/fzm/Downloads/Program source code/mysql-5.5.36");
    convertProgram("/home/fzm/Downloads/Program source code/mysql-5.5.36");
    //deleteTempTable();
    stopMysql();
    
    return 0;
}