#include "mysqlOper.h"
#include "xmlOper.h"
#include "sourceOper.h"
#include "strOper.h"
#include "buildFuncLibrary.h"

int main(int argc, char **argv) 
{
    /*
    getProgramName("/home/fzm/Downloads/Program source code/httpd-2.4.27");
    convertProgram("/home/fzm/Downloads/Program source code/httpd-2.4.27");
    deleteDir("temp");
    */
    if(buildLibrary())
        printf("create function library success!\n");
    else
        printf("create function library failure!\n");
    return 0;
}