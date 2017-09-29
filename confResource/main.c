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
    getSoftWareConfInfo();
    //getProgramName("/home/fzm/Downloads/Program source code/redis-4.0.1");
    getProgramName(srcPath);
    initTableName();
    time(&start);

    if(rebuild)
    {
        //initSoftware("/home/fzm/Downloads/Programsourcecode/mysql-5.5.36");
        initSoftware(srcPath);
        time(&end);
        finish = end - start;
        printf("build time is: %d second\n", (int)finish);
    }
    char temp_dir[DIRPATH_MAX];
    memset(temp_dir, 0, DIRPATH_MAX);
    sprintf(temp_dir, "temp_%s", programName);
                    
    //char *confArray[] = {"server.rdb_compression", "server.rdb_checksum", "server.maxclients", "server.hz", "server.maxmemory", "server.save" };
    //char *confArray[] = {"ap_daemons_to_start", "ap_daemons_limit", "server_limit", "max_workers", "threads_per_child", "ap_threads_per_child" };
    //char *confArray[] = {"share->max_rows", "key_cache->param_buff_size", "thd->variables.max_heap_table_size", "thd->variables.sortbuff_size"};
    //char *confArray[] = {"server.rdb_compression"};
    for(currentConfOpt = beginConfOpt; currentConfOpt != NULL; currentConfOpt = currentConfOpt->next)
    {
        time(&start); 
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "----------analysing configuration(%s) use resource info-----------\n", currentConfOpt->confName);
        RecordLog(log_info);
        int i;
        confScore resultScore;
        memset(&resultScore, 0, sizeof(confScore));
        for( i = 0; i < currentConfOpt->mapVariableNum; i++)
        {
            confScore ret = buildConfScore(currentConfOpt->confVarName[i], temp_dir);
            resultScore.CPU += ret.CPU;
            resultScore.IO += ret.IO;
            resultScore.MEM += ret.MEM;
            resultScore.NET += ret.NET;
        }
        
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "CPU: %d MEM: %d IO: %d NET: %d\n", resultScore.CPU, resultScore.MEM, resultScore.IO, resultScore.NET);
        RecordLog(log_info);
        time(&end); 
        finish = end - start;
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "time is: %d second\n", (int)finish);
        RecordLog(log_info);
        RecordLog("------complete--------\n\n");
    }
    
    stopMysql();
    
    
    return 0;
}