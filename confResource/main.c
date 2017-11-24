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

    int i = 0;
    for(currentConfOpt = beginConfOpt; currentConfOpt != NULL; currentConfOpt = currentConfOpt->next)
    {
        currentAnalyzeConfOptPthreadID = 0;
        for(i = 0; i < MAX_ANALYZE_CONFOPT_PTHREAD_NUM; i++)
            analyzeConfOptPthreadRet[i] = -1;
        time(&start); 
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "----------analysing configuration(%s) use resource info-----------\n", currentConfOpt->confName);
        Result(log_info);
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
        void *pthread_ret = NULL;
        for(i = 0; i < MAX_ANALYZE_CONFOPT_PTHREAD_NUM; i++)
        {
            if(analyzeConfOptPthreadRet[i] == 0)
            {
                pthread_join(analyzeConfOptPthreadID[i], &pthread_ret);
                if(pthread_ret != NULL)
                {
                    confScore *temp_ret = (confScore *)pthread_ret;
                    resultScore.CPU += temp_ret->CPU;
                    resultScore.MEM += temp_ret->MEM;
                    resultScore.IO += temp_ret->IO;
                    resultScore.NET += temp_ret->NET;
                    free(pthread_ret);
                }
            }
        }
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "CPU: %d MEM: %d IO: %d NET: %d\n", resultScore.CPU, resultScore.MEM, resultScore.IO, resultScore.NET);
        Result(log_info);
        time(&end); 
        finish = end - start;
        memset(log_info, 0, LOGINFO_LENGTH);
        sprintf(log_info, "time is: %d second\n", (int)finish);
        Result(log_info);
        Result("------complete--------\n\n");
    }

    stopMysql();
    
    return 0;
}