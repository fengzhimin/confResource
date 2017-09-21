#include "mysqlOper.h"
#include "CXmlOper.h"
#include "CPPXmlOper.h"
#include "sourceOper.h"
#include "strOper.h"
#include "config.h"
#include "logOper.h"
#include "buildFuncLibrary.h"
#include "buildTempTable.h"

static char log_info[LOGINFO_LENGTH];

int main(int argc, char **argv) 
{
    char config_value[CONFIG_VALUE_MAX_NUM];
     time_t start, end, finish;
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
    //getProgramName("/home/fzm/Downloads/Program source code/redis-4.0.1");
    getProgramName("/home/fzm/Downloads/Programsourcecode/mysql-5.5.36");
    initTableName();
    time(&start); 
    if(strcasecmp(str, "yes") == 0)
    {
        initSoftware("/home/fzm/Downloads/Programsourcecode/mysql-5.5.36");
        time(&end);
        finish = end - start;
        printf("build time is: %d second\n", (int)finish);
    }
    char temp_dir[DIRPATH_MAX];
    memset(temp_dir, 0, DIRPATH_MAX);
    sprintf(temp_dir, "temp_%s", programName);
                    
    /*
    ExtractGlobalVarDef("/home/fzm/confResource/confResource/Debug/temp_redis-4.0.1/src/adlist.c.xml");
    ExtractFuncVarDef("/home/fzm/confResource/confResource/Debug/temp_redis-4.0.1/src/adlist.c.xml");
    ExtractFuncVarUsedInfo("/home/fzm/confResource/confResource/Debug/temp_redis-4.0.1/src/adlist.c.xml");
    */
    //scanConfVar("var", "/home/fzm/confResource/confResource/Debug/childinfo.c.xml");
    //Sclice("server.maxmemory", "/home/fzm/confResource/confResource/Debug/temp_redis-4.0.1/src/config.c.xml");
    //ExtractClassInheritFromCPPXML("/home/fzm/confResource/confResource/Debug/item_timefunc.E.cc.xml");
    //buildTempTable();
    //ExtractFuncFromCPPXML("/home/fzm/confResource/confResource/Debug/sql_lex.E.cc.xml");
    //ExtractGlobalVarDef("/home/fzm/confResource/confResource/Debug/config.E.c.xml");
    //getVarInfluence("server.maxmemory", temp_dir);
    //char *confArray[] = {"server.rdb_compression", "server.rdb_checksum", "server.maxclients", "server.hz", "server.maxmemory", "server.save" };
    //char *confArray[] = {"ap_daemons_to_start", "ap_daemons_limit", "server_limit", "max_workers", "threads_per_child", "ap_threads_per_child" };
    char *confArray[] = {"share->max_rows", "key_cache->param_buff_size", "thd->variables.max_heap_table_size", "thd->variables.sortbuff_size"};
    //char *confArray[] = {"server.rdb_compression"};
    int i;
    for(i = 0; i < 1; i++)
    {
        time(&start); 
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "----------analysing variable(%s) use resource info-----------\n", confArray[i]);
        //printf(log_info);
        RecordLog(log_info);
        //getVarUsedFunc("maxmemory", temp_dir);
        confScore ret = buildConfScore(confArray[i], temp_dir);
        //confScore ret =  getFuncScore("configSetCommand");
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "CPU: %d MEM: %d IO: %d NET: %d\n", ret.CPU, ret.MEM, ret.IO, ret.NET);
        //printf(log_info);
        RecordLog(log_info);
        time(&end); 
        finish = end - start;
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "time is: %d second\n", (int)finish);
        //printf(log_info);
        RecordLog(log_info);
        //printf("------complete--------\n");
        RecordLog("------complete--------\n\n");
    }
    
    stopMysql();
    
    
    return 0;
}