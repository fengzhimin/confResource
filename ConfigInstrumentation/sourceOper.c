/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : sourceOper.c
* Descripe     : parse source code
******************************************************/

#include "sourceOper.h"

static char error_info[LOGINFO_LENGTH];
static char xml_dir[DIRPATH_MAX];

bool getSoftWareConfInfo()
{
    if(!getConfValueByLabelAndKey("environmentConf", "srcPath", srcPath))
    {
        Error("get config info failure! label('environmentConf') key('srcPath').\n");
		return false;
    }
    
    char value[CONFIG_VALUE_MAX_NUM];
    memset(value, 0, CONFIG_VALUE_MAX_NUM*sizeof(char));
    //获取需要被插桩的配置项总个数
    if(!getConfValueByLabelAndKey("configInfo", "configNum", value))
    {
        Error("get config info failure! label('configInfo') key('configNum').\n");
		return false;
    }
    else
    {
        totalHandledConfNum = StrToInt(value);
    }
    
    //获取需要被插桩的每个配置项
    memset(value, 0, CONFIG_VALUE_MAX_NUM*sizeof(char));
    char (*confName)[MAX_SUBSTR] = malloc(MAX_SUBSTR*totalHandledConfNum);
    if(!getConfValueByLabelAndKey("configInfo", "configName", value))
    {
        Error("get config info failure! label('configInfo') key('configName').\n");
		return false;
    }
    else
    {
        cutStrByLabel(value, ':', confName, totalHandledConfNum);
    }
    
    //申请所有被插桩的配置项
    handledConfiguration = malloc(sizeof(HandledConf)*totalHandledConfNum);
    char label[CONFIG_LABEL_MAX_NUM];
    char key[CONFIG_KEY_MAX_NUM];
    memset(label, 0, CONFIG_LABEL_MAX_NUM*sizeof(char));
    memset(key, 0, CONFIG_KEY_MAX_NUM*sizeof(char));
    int insertNum = 0;
    for(int index = 1; index <= totalHandledConfNum; index++)
    {
        strcpy(label, confName[index-1]);
        strcpy(handledConfiguration[index-1].confName, label);
        memset(value, 0, CONFIG_VALUE_MAX_NUM*sizeof(char));
        if(!getConfValueByLabelAndKey(label, "InsertNum", value))
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "get config info failure! label('%s') key('InsertNum').\n", label);
            Error(error_info);
            free(confName);
            return false;
        }
        else
        {
            insertNum = StrToInt(value);
            //申请每个需要被插桩的配置项所需插桩的位置
            handledConfiguration[index-1].insertInfo = malloc(sizeof(InstrumentInfo)*insertNum);
            for(int i = 1; i <= insertNum; i++)
            {
                //get source path from config file
                sprintf(key, "srcPath_%d", i);
                memset(value, 0, CONFIG_VALUE_MAX_NUM*sizeof(char));
                if(!getConfValueByLabelAndKey(label, key, value))
                {
                    memset(error_info, 0, LOGINFO_LENGTH);
                    sprintf(error_info, "get config info failure! label('%s') key('%s').\n", label, key);
                    Error(error_info);
                    free(confName);
                    return false;
                }
                else
                {
                    strcpy(handledConfiguration[index-1].insertInfo[i-1].srcPath, value);
                }
                
                //get function name from config file
                sprintf(key, "funcName_%d", i);
                memset(value, 0, CONFIG_VALUE_MAX_NUM*sizeof(char));
                if(!getConfValueByLabelAndKey(label, key, value))
                {
                    memset(error_info, 0, LOGINFO_LENGTH);
                    sprintf(error_info, "get config info failure! label('%s') key('%s').\n", label, key);
                    Error(error_info);
                    free(confName);
                    return false;
                }
                else
                {
                    strcpy(handledConfiguration[index-1].insertInfo[i-1].funcName, value);
                }
            }
        }
    }
    
    free(confName);
    
	return true;
}

bool getProgramName(char *sourcePath)
{
    int index = ExtractLastCharIndex(sourcePath, '/');
    int length = strlen(sourcePath);
    if(index != -1)
    {
        if((length - index) > MAX_PROGRAMNAME_NUM)
        {
            Error("program name greater than preset values\n");
            return false;
        }
        else
            strcpy(programName, (char *)&sourcePath[index+1]);
    }
    else
    {
        if(length > MAX_PROGRAMNAME_NUM)
        {
            Error("program name greater than preset values\n");
            return false;
        }
        else
            strcpy(programName, sourcePath);
    }
    
    return true;
}

int judgeSrcFileType(char *filePath)
{
    if(strchr(filePath, '.') == NULL)
        return -1;
    
    //judge C language file
    if(strlen(filePath) <= 2)
        return -1;
    if(strcmp((char *)&filePath[strlen(filePath)-2], ".c") == 0)
        return 1;
    
    //judge C++ language file
    if(strlen(filePath) <= 3)
        return -1;
    if(strcasecmp((char *)&filePath[strlen(filePath)-3], ".cc") == 0)
        return 2;
    if(strlen(filePath) <= 4)
        return -1;
    if(strcasecmp((char *)&filePath[strlen(filePath)-4], ".cpp") == 0)
        return 2;
    if(strcasecmp((char *)&filePath[strlen(filePath)-4], ".cxx") == 0)
        return 2;
    if(strcasecmp((char *)&filePath[strlen(filePath)-4], ".c++") == 0)
        return 2;
        
    //judge Java language file
    if(strlen(filePath) <= 5)
        return -1;
    if(strcmp((char *)&filePath[strlen(filePath)-2], ".java") == 0)
        return 3;
    
    return -1;
}

int judgeXmlFileType(char *filePath)
{
    if(strchr(filePath, '.') == NULL)
        return -1;
    
    //judge whether C language source xml file or not
    if(strlen(filePath) <= 6)
        return -1;
    if(strcmp((char *)&filePath[strlen(filePath)-6], ".c.xml") == 0)
        return 1;
        
    //judge whether C++ language source xml file or not
    if(strlen(filePath) <= 7)
        return -1;
    if(strcasecmp((char *)&filePath[strlen(filePath)-7], ".cc.xml") == 0)
        return 2;
    if(strlen(filePath) <= 8)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".cpp.xml") == 0)
        return 2;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".cxx.xml") == 0)
        return 2;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".c++.xml") == 0)
        return 2;
        
    //judge whether Java language source xml file or not
    if(strlen(filePath) <= 9)
        return -1;
    if(strcmp((char *)&filePath[strlen(filePath)-6], ".java.xml") == 0)
        return 3;
        
    return -1;
}

bool ExecSrcML(char *srcPath, char *desPath)
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
        if(execlp("srcml", "srcml", srcPath, "-o", desPath, NULL) < 0)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "convert %s to XML failed: %s.\n", srcPath, strerror(errno));
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
        else if(status == 0)   //srcml execute success return 0
            return true;
        else
            return false;
    }
    
    return false;
}

bool CpyFile(char *srcPath, char *desPath)
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
        if(execlp("cp", "cp", srcPath, desPath, NULL) < 0)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "copy %s to %s failed: %s.\n", srcPath, desPath, strerror(errno));
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
        else if(status == 0)   //cp execute success return 0
            return true;
        else
            return false;
    }
    
    return false;
}

int getTotalConvertFileNum(char *dirPath)
{
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    int ret = 0;
    
    char child_dir[DIRPATH_MAX];
    pdir = opendir(dirPath);
    if(pdir)
    {
        while((pdirent = readdir(pdir)) != NULL)
        {
            //跳过"."和".."和隐藏文件夹
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 || (pdirent->d_name[0] == '.'))
                continue;
            
            memset(child_dir, 0, DIRPATH_MAX);
            sprintf(child_dir, "%s/%s", dirPath, pdirent->d_name);
            if(lstat(child_dir, &statbuf) < 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "lstat %s to failed: %s.\n", child_dir, strerror(errno));
                Error(error_info);
                closedir(pdir);
                
                return 0;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                ret += getTotalConvertFileNum(child_dir);
            }
            if(S_ISREG(statbuf.st_mode))
            {
                if(judgeSrcFileType(child_dir) != -1)
                {
                    ret++;
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", dirPath, strerror(errno));
        Error(error_info);
    }
    closedir(pdir);
    
    return ret;
}

static void *pthread_handle_Convert(void *arg)
{
    int *ret = malloc(sizeof(int));
    convertPthread_arg *argument = (convertPthread_arg *)arg;
    if(ExecSrcML(argument->src_dir, argument->des_dir))
        *ret = 1;
    else
        *ret = 0;
    pthread_exit((void *)ret);
}

bool SrcToXML(char *dirPath)
{
    char temp_dir[DIRPATH_MAX];
    char srcCpy_dir[DIRPATH_MAX];
    char *temp_point = strstr(dirPath, programName);
    if(temp_point[strlen(programName)] == '\0')
    {
        memset(temp_dir, 0, DIRPATH_MAX);
        sprintf(temp_dir, "temp_%s", programName);
    }
    else
    {
        //递归遍历文件夹
        temp_point = (char *)&temp_point[strlen(programName)];
        memset(temp_dir, 0, DIRPATH_MAX);
        sprintf(temp_dir, "temp_%s%s", programName, temp_point);
    }
    //创建存放xml文件的临时文件夹
    createDir(temp_dir);
    //创建源码的拷贝文件夹
    memset(srcCpy_dir, 0, DIRPATH_MAX);
    strcpy(srcCpy_dir, &(temp_dir[5]));
    createDir(srcCpy_dir);
    
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    bool ret = true;
    
    char child_dir[DIRPATH_MAX];
    pdir = opendir(dirPath);
    if(pdir)
    {
        while((pdirent = readdir(pdir)) != NULL)
        {
            //跳过"."和".."和隐藏文件夹
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 || (pdirent->d_name[0] == '.'))
                continue;
            
            memset(child_dir, 0, DIRPATH_MAX);
            sprintf(child_dir, "%s/%s", dirPath, pdirent->d_name);
            if(lstat(child_dir, &statbuf) < 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "lstat %s to failed: %s.\n", child_dir, strerror(errno));
                Error(error_info);
                closedir(pdir);
                
                return false;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                if(SrcToXML(child_dir))
                    ret = true;
                else
                {
                    ret = false;
                    closedir(pdir);
                    return ret;
                }
            }
            if(S_ISREG(statbuf.st_mode))
            {
                if(judgeSrcFileType(child_dir) != -1)
                {
                    memset(xml_dir, 0, DIRPATH_MAX);
                    sprintf(xml_dir, "%s/%s.xml", temp_dir, pdirent->d_name);
                    
                    void *pthread_ret = NULL;
                    if(ConvertSRCPthreadRet[currentConvertSrcPthreadID] == 0)
                    {
                        pthread_join(convertSRCPthreadID[currentConvertSrcPthreadID], &pthread_ret);
                        if(pthread_ret != NULL)
                        {
                            ret = *(int *)pthread_ret;
                            free(pthread_ret);
                            if(!ret)
                            {
                                Error("pthread_join convert src failure!\n");
                                closedir(pdir);
                                return ret;
                            }
                        }
                    }
                    
                    curConvertFileNum++;
                    printf("convert src file %s(%d/%d)\n", child_dir, curConvertFileNum, totalConvertFileNum);
                    
                    memset(&convSrcPthreadArg[currentConvertSrcPthreadID], 0, sizeof(convertPthread_arg));
                    strcpy(convSrcPthreadArg[currentConvertSrcPthreadID].des_dir, xml_dir);
                    strcpy(convSrcPthreadArg[currentConvertSrcPthreadID].src_dir, child_dir);
                    convSrcPthreadArg[currentConvertSrcPthreadID].pthreadID = 0;
                    ConvertSRCPthreadRet[currentConvertSrcPthreadID] = pthread_create(&convertSRCPthreadID[currentConvertSrcPthreadID], \
                    NULL, pthread_handle_Convert, (void *)&convSrcPthreadArg[currentConvertSrcPthreadID]);
                    
                    currentConvertSrcPthreadID++;
                    currentConvertSrcPthreadID = currentConvertSrcPthreadID%MAX_CONVERT_SRC_PTHREAD_NUM;
                    
                    
                }
                else
                {
                    //拷贝非源文件
                    memset(xml_dir, 0, DIRPATH_MAX);
                    sprintf(xml_dir, "%s/%s", srcCpy_dir, pdirent->d_name);
                    CpyFile(child_dir, xml_dir);
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", dirPath, strerror(errno));
        Error(error_info);
        
        ret = false;
    }
    closedir(pdir);
    
    return ret;
}

bool XMLToSrc(char *dirPath)
{
    char temp_dir[DIRPATH_MAX];
    char *temp_point = strstr(dirPath, programName);
    if(temp_point[strlen(programName)] == '\0')
    {
        memset(temp_dir, 0, DIRPATH_MAX);
        sprintf(temp_dir, "%s", programName);
    }
    else
    {
        //递归调用
        temp_point = (char *)&temp_point[strlen(programName)];
        memset(temp_dir, 0, DIRPATH_MAX);
        sprintf(temp_dir, "%s%s", programName, temp_point);
    }
    
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    bool ret = true;
    
    char child_dir[DIRPATH_MAX];
    pdir = opendir(dirPath);
    if(pdir)
    {
        while((pdirent = readdir(pdir)) != NULL)
        {
            //跳过"."和".."和隐藏文件夹
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 || (pdirent->d_name[0] == '.'))
                continue;
            
            memset(child_dir, 0, DIRPATH_MAX);
            sprintf(child_dir, "%s/%s", dirPath, pdirent->d_name);
            if(lstat(child_dir, &statbuf) < 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "lstat %s to failed: %s.\n", child_dir, strerror(errno));
                Error(error_info);
                closedir(pdir);
                
                return false;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                if(XMLToSrc(child_dir))
                    ret = true;
                else
                {
                    ret = false;
                    closedir(pdir);
                    return ret;
                }
            }
            if(S_ISREG(statbuf.st_mode))
            {
                if(judgeXmlFileType(child_dir) != -1)
                {
                    
                    memset(xml_dir, 0, DIRPATH_MAX);
                    sprintf(xml_dir, "%s/%s", temp_dir, pdirent->d_name);
                    
                    void *pthread_ret = NULL;
                    if(ConvertXMLPthreadRet[currentConvertXmlPthreadID] == 0)
                    {
                        pthread_join(convertXMLPthreadID[currentConvertXmlPthreadID], &pthread_ret);
                        if(pthread_ret != NULL)
                        {
                            ret = *(int *)pthread_ret;
                            free(pthread_ret);
                            
                            if(!ret)
                            {
                                Error("pthread_join convert xml failure!\n");
                                closedir(pdir);
                                return ret;
                            }
                        }
                    }
                    
                    curConvertFileNum++;
                    printf("convert xml file %s(%d/%d)\n", child_dir, curConvertFileNum, totalConvertFileNum);
                    
                    memset(&convXmlPthreadArg[currentConvertXmlPthreadID], 0, sizeof(convertPthread_arg));
                    //删除.xml后缀
                    strncpy(convXmlPthreadArg[currentConvertXmlPthreadID].des_dir, xml_dir, strlen(xml_dir)-4);
                    strcpy(convXmlPthreadArg[currentConvertXmlPthreadID].src_dir, child_dir);
                    convXmlPthreadArg[currentConvertXmlPthreadID].pthreadID = 0;
                    ConvertXMLPthreadRet[currentConvertXmlPthreadID] = pthread_create(&convertXMLPthreadID[currentConvertXmlPthreadID], \
                    NULL, pthread_handle_Convert, (void *)&convXmlPthreadArg[currentConvertXmlPthreadID]);
                    
                    currentConvertXmlPthreadID++;
                    currentConvertXmlPthreadID = currentConvertXmlPthreadID%MAX_CONVERT_XML_PTHREAD_NUM;
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", dirPath, strerror(errno));
        Error(error_info);
        
        ret = false;
    }
    closedir(pdir);
    
    return ret;
}

static void *pthread_handle_Insert(void *arg)
{
    int *ret = malloc(sizeof(int));
    char *argument = (char *)arg;
    if(InsertCode(argument))
        *ret = 1;
    else
        *ret = 0;
    pthread_exit((void *)ret);
}

bool InsertXML(char *dirPath)
{
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    bool ret = true;
    
    char child_dir[DIRPATH_MAX];
    pdir = opendir(dirPath);
    if(pdir)
    {
        while((pdirent = readdir(pdir)) != NULL)
        {
            //跳过"."和".."和隐藏文件夹
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 || (pdirent->d_name[0] == '.'))
                continue;
            
            memset(child_dir, 0, DIRPATH_MAX);
            sprintf(child_dir, "%s/%s", dirPath, pdirent->d_name);
            if(lstat(child_dir, &statbuf) < 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "lstat %s to failed: %s.\n", child_dir, strerror(errno));
                Error(error_info);
                closedir(pdir);
                
                return false;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                if(InsertXML(child_dir))
                    ret = true;
                else
                {
                    ret = false;
                    closedir(pdir);
                    return ret;
                }
            }
            if(S_ISREG(statbuf.st_mode))
            {
                if(judgeXmlFileType(child_dir) != -1)
                {
                    void *pthread_ret = NULL;
                    if(InsertXMLPthreadRet[currentInsertXmlPthreadID] == 0)
                    {
                        pthread_join(insertXMLPthreadID[currentInsertXmlPthreadID], &pthread_ret);
                        if(pthread_ret != NULL)
                        {
                            ret = *(int *)pthread_ret;
                            free(pthread_ret);
                            if(!ret)
                            {
                                Error("pthread_join insert xml failure!\n");
                                closedir(pdir);
                                return ret;
                            }
                        }
                    }
                    
                    curConvertFileNum++;
                    printf("insert xml file %s(%d/%d)\n", child_dir, curConvertFileNum, totalConvertFileNum);
                    
                    memset(insXmlPthreadArg[currentInsertXmlPthreadID], 0, DIRPATH_MAX);
                    strcpy(insXmlPthreadArg[currentInsertXmlPthreadID], child_dir);
                    
                    InsertXMLPthreadRet[currentInsertXmlPthreadID] = pthread_create(&insertXMLPthreadID[currentInsertXmlPthreadID], \
                    NULL, pthread_handle_Insert, (void *)insXmlPthreadArg[currentInsertXmlPthreadID]);
                    
                    currentInsertXmlPthreadID++;
                    currentInsertXmlPthreadID = currentInsertXmlPthreadID%MAX_INSERT_XML_PTHREAD_NUM;
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", dirPath, strerror(errno));
        Error(error_info);
        
        ret = false;
    }
    closedir(pdir);
    
    return ret;
}

bool BuildSrcToXml()
{
    totalConvertFileNum = getTotalConvertFileNum(srcPath);
    int i;
    
    for(i = 0; i < MAX_CONVERT_SRC_PTHREAD_NUM; i++)
        ConvertSRCPthreadRet[i] = -1;
    curConvertFileNum = 0;
    currentConvertSrcPthreadID = 0;
    bool ret = SrcToXML(srcPath);
    for(i = 0; i < MAX_CONVERT_SRC_PTHREAD_NUM; i++)
    {
        void *pthread_ret = NULL;
        if(ConvertSRCPthreadRet[i] == 0)
        {
            pthread_join(convertSRCPthreadID[i], &pthread_ret);
            if(pthread_ret != NULL)
            {
                ret = *(int *)pthread_ret;
                free(pthread_ret);
                if(!ret)
                {
                    Error("pthread_join convert src failure!\n");
                }
            }
        }
    }
    
    return ret;
}

bool BuildXmlToSrc()
{
    int i;
    for(i = 0; i < MAX_CONVERT_XML_PTHREAD_NUM; i++)
        ConvertXMLPthreadRet[i] = -1;
    curConvertFileNum = 0;
    currentConvertXmlPthreadID = 0;
    char xml_dir[DIRPATH_MAX] = "";
    memset(xml_dir, 0, DIRPATH_MAX);
    sprintf(xml_dir, "temp_%s", programName);
    bool ret = XMLToSrc(xml_dir);
    for(i = 0; i < MAX_CONVERT_XML_PTHREAD_NUM; i++)
    {
        void *pthread_ret = NULL;
        if(ConvertXMLPthreadRet[i] == 0)
        {
            pthread_join(convertXMLPthreadID[i], &pthread_ret);
            if(pthread_ret != NULL)
            {
                ret = *(int *)pthread_ret;
                free(pthread_ret);
                if(!ret)
                {
                    Error("pthread_join convert xml failure!\n");
                }
            }
        }
    }
    
    return ret;
}

bool BuildInsertXml()
{
    int i;
    for(i = 0; i < MAX_INSERT_XML_PTHREAD_NUM; i++)
        InsertXMLPthreadRet[i] = -1;
    curConvertFileNum = 0;
    currentInsertXmlPthreadID = 0;
    char xml_dir[DIRPATH_MAX];
    memset(xml_dir, 0, DIRPATH_MAX);
    sprintf(xml_dir, "temp_%s", programName);
    bool ret = InsertXML(xml_dir);
    for(i = 0; i < MAX_INSERT_XML_PTHREAD_NUM; i++)
    {
        void *pthread_ret = NULL;
        if(InsertXMLPthreadRet[i] == 0)
        {
            pthread_join(insertXMLPthreadID[i], &pthread_ret);
            if(pthread_ret != NULL)
            {
                ret = *(int *)pthread_ret;
                free(pthread_ret);
                if(!ret)
                {
                    Error("pthread_join insert xml failure!\n");
                }
            }
        }
    }
    
    return ret;
}

void ClearTmp()
{
    char xml_dir[DIRPATH_MAX];
    memset(xml_dir, 0, DIRPATH_MAX);
    sprintf(xml_dir, "temp_%s", programName);
    
    deleteDir(xml_dir);
    
    //释放内存
    for(int index = 0; index < totalHandledConfNum; index++)
    {
        free(handledConfiguration[index].insertInfo);
    }
    
    free(handledConfiguration);
}