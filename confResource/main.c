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
static char xml_dir[DIRPATH_MAX];
static pthread_mutex_t mutex;

static void *pthread_handle_confOpt(void *arg);

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
    
    memset(xml_dir, 0, DIRPATH_MAX);
    sprintf(xml_dir, "temp_%s", programName);
                    
    //char *confArray[] = {"server.rdb_compression", "server.rdb_checksum", "server.maxclients", "server.hz", "server.maxmemory", "server.save" };
    //char *confArray[] = {"ap_daemons_to_start", "ap_daemons_limit", "server_limit", "max_workers", "threads_per_child", "ap_threads_per_child" };
    //char *confArray[] = {"share->max_rows", "key_cache->param_buff_size", "thd->variables.max_heap_table_size", "thd->variables.sortbuff_size"};
    //char *confArray[] = {"server.rdb_compression"};
    pthread_mutex_init(&mutex, NULL);
    //pthread_t *pthreadID = malloc(sizeof(pthread_t)*confOptNum);
    int pthread_ret, i = 0;
    for(currentConfOpt = beginConfOpt, i = 0; currentConfOpt != NULL; currentConfOpt = currentConfOpt->next, i++)
    {
        currentPthreadID = 0;
        for(i = 0; i < MAX_PTHREAD_NUM; i++)
            pthreadRet[i] = -1;
        time(&start); 
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "----------analysing configuration(%s) use resource info-----------\n", currentConfOpt->confName);
        RecordLog(log_info);
        confScore resultScore;
        memset(&resultScore, 0, sizeof(confScore));
        for( i = 0; i < currentConfOpt->mapVariableNum; i++)
        {
            confScore ret = buildConfScore(currentConfOpt->confVarName[i], xml_dir);
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

        for(i = 0; i < MAX_PTHREAD_NUM; i++)
        {
            if(pthreadRet[i] == 0)
                pthread_join(pthreadID[i], NULL);
        }
        /*
        pthread_ret = pthread_create(&(pthreadID[i]), NULL, pthread_handle_confOpt, (void *)currentConfOpt);
        if(pthread_ret != 0)
        {
            printf("create pthread ID(%d) failure!\n", i);
        }
        */
    }
    /*
    for(i = 0; i < confOptNum; i++)
        pthread_join(pthreadID[i], NULL);
    pthread_mutex_destroy(&mutex);
    free(pthreadID);
    */
    stopMysql();
    
    
    return 0;
}

static void *pthread_handle_confOpt(void *arg)
{
    confOptMap *confOpt = (confOptMap *)arg;
    time_t start, end, finish;
    time(&start); 
    int i;
    confScore resultScore;
    memset(&resultScore, 0, sizeof(confScore));
    for( i = 0; i < confOpt->mapVariableNum; i++)
    {
        confScore ret = buildConfScore(confOpt->confVarName[i], xml_dir);
        resultScore.CPU += ret.CPU;
        resultScore.IO += ret.IO;
        resultScore.MEM += ret.MEM;
        resultScore.NET += ret.NET;
    }
    pthread_mutex_lock(&mutex);
    memset(log_info, 0, LOGINFO_LENGTH);
    sprintf(log_info, "----------analysing configuration(%s) use resource info-----------\n", confOpt->confName);
    RecordLog(log_info);
    memset(log_info, 0, LOGINFO_LENGTH);
    sprintf(log_info, "CPU: %d MEM: %d IO: %d NET: %d\n", resultScore.CPU, resultScore.MEM, resultScore.IO, resultScore.NET);
    RecordLog(log_info);
    time(&end); 
    finish = end - start;
    memset(log_info, 0, LOGINFO_LENGTH);
    sprintf(log_info, "time is: %d second\n", (int)finish);
    RecordLog(log_info);
    RecordLog("------complete--------\n\n");
    pthread_mutex_unlock(&mutex);
}