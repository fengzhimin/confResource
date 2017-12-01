/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : PreCompileOper.c
* Descripe     : execute precompile command operation
******************************************************/

#include "PreCompileOper.h"

static char error_info[LOGINFO_LENGTH];

bool ExecCommand(char *shellPath)
{
    pid_t pid = fork();
    if(pid < 0)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "create process failed: %s.\n", strerror(errno));
        Error(error_info);
        return false;
    }
    else if(pid == 0)
    {
        if(execlp("sh", "sh", shellPath, NULL) < 0)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "execute shell(%s) failed: %s.\n", shellPath, strerror(errno));
            Error(error_info);

            exit(-1);
        }
    }
    else
    {
        int status;
        if(wait(&status) < 0)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "wait child process exit failed: %s.\n", strerror(errno));
            Error(error_info);
            
            return false;
        }
        else if(status == -1)
            return false;
        else
            return true;
    }
    
    return false;
}

static void *pthread_handle_PreCompile(void *arg)
{
    int *ret = malloc(sizeof(int));
    //参数为shell脚本
    char *argument = (char *)arg;
    if(ExecCommand(argument))
        *ret = 1;
    else
        *ret = 0;
    pthread_exit((void *)ret);
}

bool ExecPreCompile(PreCompileInfoList *CommandInfoList)
{
    int i;
    
    for(i = 0; i < MAX_EXECPRECOMPILE_PTHREAD_NUM; i++)
    {
        exeCPreCompilePthreadRet[i] = -1;
        sprintf(shellPath[i], "build_PreCompile%d.sh", i);
    }
    curPreCompileFileNum = 0;
    currentPreCompilePthreadID = 0;
    
    PreCompileInfoList *current = CommandInfoList;
    int fd;
    int ret;
    while(current != NULL)
    {
        void *pthread_ret = NULL;
        if(exeCPreCompilePthreadRet[currentPreCompilePthreadID] == 0)
        {
            pthread_join(exeCPreCompilePthreadID[currentPreCompilePthreadID], &pthread_ret);
            if(pthread_ret != NULL)
            {
                ret = *(int *)pthread_ret;
                free(pthread_ret);
                if(!ret)
                {
                    Error("pthread_join precompile failure!\n");
                    return false;
                }
            }
        }
        curPreCompileFileNum++;
        printf("PreCompile file: %s/%s...(%d/%d)\n", current->info.dirPath, current->info.fileName, curPreCompileFileNum, totalPreCompileFileNum);
        
        //将要执行的命令写入shell脚本中
        fd = OpenFile(shellPath[currentPreCompilePthreadID], O_RDWR | O_TRUNC);
        WriteFile(fd, "#!/bin/bash\ncd ");
        WriteFile(fd, current->info.dirPath);
        WriteFile(fd, "\n");
        WriteFile(fd, current->info.preCompileCommand);
        CloseFile(fd);
        
        exeCPreCompilePthreadRet[currentPreCompilePthreadID] = pthread_create(&exeCPreCompilePthreadID[currentPreCompilePthreadID], \
                        NULL, pthread_handle_PreCompile, (void *)shellPath[currentPreCompilePthreadID]);
                        
        currentPreCompilePthreadID++;
        currentPreCompilePthreadID = currentPreCompilePthreadID%MAX_EXECPRECOMPILE_PTHREAD_NUM;
    
        current = current->next;
    }

    for(i = 0; i < MAX_EXECPRECOMPILE_PTHREAD_NUM; i++)
    {
        void *pthread_ret = NULL;
        if(exeCPreCompilePthreadRet[i] == 0)
        {
            pthread_join(exeCPreCompilePthreadID[i], &pthread_ret);
            if(pthread_ret != NULL)
            {
                ret = *(int *)pthread_ret;
                free(pthread_ret);
                if(!ret)
                {
                    Error("pthread_join precompile failure!\n");
                    return false;
                }
            }
        }
    }
    
    return true;
}