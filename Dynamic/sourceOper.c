/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : sourceOper.c
* Descripe     : parse source code
******************************************************/

#include "sourceOper.h"

static char subStr2[2][MAX_SUBSTR];
static char lineData[LINE_CHAR_MAX_NUM];
static char error_info[LOGINFO_LENGTH];
static char xml_dir[DIRPATH_MAX];

bool getSoftWareConfInfo()
{
    int fd = OpenFile(INPUT_PATH, O_RDONLY);
	if(fd == -1)
	{
		return false;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	while(ReadLine(fd, lineData) == -1)
	{
		removeBeginSpace(lineData);
        //remove note line
        if(lineData[0] == '#')
            goto next;
		int specificCharNum = getSpecCharNumFromStr(lineData, '=');
        if(specificCharNum == 1)
        {
            //handle srcPath and rebuild option
            cutStrByLabel(lineData, '=', subStr2, 2);
            if(strcasecmp(subStr2[0], "srcPath") == 0)
            {
                //get srcPath value
                strcpy(srcPath, subStr2[1]);
                break;
            }
        }
next:
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	CloseFile(fd);
    
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

bool judgeCSrcFile(char *filePath)
{
    if(strlen(filePath) <= 2)
        return false;
    if(strcmp((char *)&filePath[strlen(filePath)-2], ".c") == 0)
        return true;
    else
        return false;
}

bool judgeCSrcXmlFile(char *filePath)
{
    if(strlen(filePath) <= 6)
        return false;
    if(strcmp((char *)&filePath[strlen(filePath)-6], ".c.xml") == 0)
        return true;
    else
        return false;
}

bool judgeCPPSrcFile(char *filePath)
{
    if(strlen(filePath) <= 3)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-3], ".cc") == 0)
        return true;
    if(strlen(filePath) <= 4)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-4], ".cpp") == 0)
        return true;
    if(strcasecmp((char *)&filePath[strlen(filePath)-4], ".cxx") == 0)
        return true;
    if(strcasecmp((char *)&filePath[strlen(filePath)-4], ".c++") == 0)
        return true;
    return false;
}

bool judgeCPPSrcXmlFile(char *filePath)
{
    if(strlen(filePath) <= 7)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-7], ".cc.xml") == 0)
        return true;
    if(strlen(filePath) <= 8)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".cpp.xml") == 0)
        return true;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".cxx.xml") == 0)
        return true;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".c++.xml") == 0)
        return true;
    return false;
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
                if(judgeCSrcFile(child_dir) || judgeCPPSrcFile(child_dir))
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
                if(judgeCSrcFile(child_dir) || judgeCPPSrcFile(child_dir))
                {
                    //handle C language preprocess file
                    curConvertFileNum++;
                    printf("convert src file %s(%d/%d)\n", child_dir, curConvertFileNum, totalConvertFileNum);
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
                if(judgeCSrcXmlFile(child_dir) || judgeCPPSrcXmlFile(child_dir))
                {
                    //handle C language preprocess file
                    curConvertFileNum++;
                    printf("convert xml file %s(%d/%d)\n", child_dir, curConvertFileNum, totalConvertFileNum);
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
                if(judgeCSrcXmlFile(child_dir) || judgeCPPSrcXmlFile(child_dir))
                {
                    //handle C language preprocess file
                    curConvertFileNum++;
                    printf("insert xml file %s(%d/%d)\n", child_dir, curConvertFileNum, totalConvertFileNum);
                    
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
    char xml_dir[DIRPATH_MAX];
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
}