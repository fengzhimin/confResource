#include "mysqlOper.h"
#include "xmlOper.h"
#include "sourceOper.h"
#include "strOper.h"
#include "config.h"
#include "buildFuncLibrary.h"
#include "buildTempTable.h"

int main(int argc, char **argv) 
{
    char config_value[CONFIG_VALUE_MAX_NUM];
	if(getConfValueByLabelAndKey("funcCall", "num", config_value))
    {
		max_funcCallRecursive_NUM = StrToInt(config_value);
    }
    
    if(!startMysql())
    {
        printf("connect mysql failure!\n");
        return -1;
    }
    char str[10] = "";
    printf("rebuild program? yes or no: ");
    scanf("%s", str);
    getProgramName("/home/fzm/Downloads/Program source code/redis-4.0.1");
    if(strcasecmp(str, "yes") == 0)
    {
        deleteTempXMLFile();
        buildLibrary();
        buildTempTable();
        convertProgram("/home/fzm/Downloads/Program source code/redis-4.0.1");
        buildFuncScore();
    }
    char temp_dir[DIRPATH_MAX];
    memset(temp_dir, 0, DIRPATH_MAX);
    sprintf(temp_dir, "temp_%s", programName);
    char *confName = "maxclients";
    time_t start, end, finish; 
    time(&start); 
    printf("----------analysing variable(%s) use resource info-----------\n", confName);
    //getVarUsedFunc("maxmemory", temp_dir);
    confScore ret = buildConfScore(confName, temp_dir);
    //confScore ret =  getFuncScore("configSetCommand");
    printf("CPU: %d MEM: %d IO: %d NET: %d\n", ret.CPU, ret.MEM, ret.IO, ret.NET);
    printf("------complete--------\n");
    time(&end); 
    finish = end - start;
    printf("time is: %d second!\n", finish);
    stopMysql();
    
    
    return 0;
}