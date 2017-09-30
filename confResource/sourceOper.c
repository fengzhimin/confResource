/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : sourceOper.c
* Descripe     : parse source code
******************************************************/

#include "sourceOper.h"

static char error_info[LOGINFO_LENGTH];
static char xml_dir[DIRPATH_MAX];
static char sqlCommand[LINE_CHAR_MAX_NUM];
static char src_dir[DIRPATH_MAX];
static char subStr2[2][MAX_SUBSTR];
static char lineData[LINE_CHAR_MAX_NUM];

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
            }
            else if(strcasecmp(subStr2[0], "rebuild") == 0)
            {
                //get rebuild value
                if(strcasecmp(subStr2[1], "yes") == 0)
                    rebuild = true;
                else
                    rebuild = false;
            }
        }
        else
        {
            //handle config option map
            specificCharNum = getSpecCharNumFromStr(lineData, ':');
            if(specificCharNum > 0)
            {
                if(beginConfOpt == NULL)
                    beginConfOpt = endConfOpt = malloc(sizeof(confOptMap));
                else
                    endConfOpt = endConfOpt->next = malloc(sizeof(confOptMap));
                memset(endConfOpt, 0, sizeof(confOptMap));
                endConfOpt->confVarName = (char (*)[MAX_SUBSTR])malloc(specificCharNum*MAX_SUBSTR);
                cutStrByLabel(lineData, ':', subStr2, 2);
                strcpy(endConfOpt->confName, subStr2[0]);
                cutStrByLabel(subStr2[1], ':', endConfOpt->confVarName, specificCharNum);
                endConfOpt->mapVariableNum = specificCharNum;
                confOptNum++;
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
            RecordLog("program name greater than preset values\n");
            return false;
        }
        else
            strcpy(programName, (char *)&sourcePath[index+1]);
    }
    else
    {
        if(length > MAX_PROGRAMNAME_NUM)
        {
            RecordLog("program name greater than preset values\n");
            return false;
        }
        else
            strcpy(programName, sourcePath);
    }
    
    return true;
}

bool judgeCPreprocessFile(char *filePath)
{
    //strlen(".E.c") == 4
    if(strlen(filePath) <= 4)
        return false;
    if(strcmp((char *)&filePath[strlen(filePath)-4], ".E.c") == 0)
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

bool judgeCHeaderXmlFile(char *filePath)
{
    if(strlen(filePath) <= 6)
        return false;
    if(strcmp((char *)&filePath[strlen(filePath)-6], ".h.xml") == 0)
        return true;
    else
        return false;
}

bool judgeCPPPreprocessFile(char *filePath)
{
    if(strlen(filePath) <= 5)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-5], ".E.cc") == 0)
        return true;
    if(strlen(filePath) <= 6)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-6], ".E.cpp") == 0)
        return true;
    if(strcasecmp((char *)&filePath[strlen(filePath)-6], ".E.cxx") == 0)
        return true;
    if(strcasecmp((char *)&filePath[strlen(filePath)-6], ".E.c++") == 0)
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

bool judgeCPPHeaderXmlFile(char *filePath)
{
    if(strlen(filePath) <= 7)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-7], ".hh.xml") == 0)
        return true;
    if(strlen(filePath) <= 8)
        return false;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".hpp.xml") == 0)
        return true;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".hxx.xml") == 0)
        return true;
    if(strcasecmp((char *)&filePath[strlen(filePath)-8], ".h++.xml") == 0)
        return true;
    return false;
}

bool CodeToXML(char *srcPath, char *desPath)
{
    pid_t pid = fork();
    if(pid < 0)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "create process failed: %s.\n", strerror(errno));
        RecordLog(error_info);
        return false;
    }
    else if(pid == 0)
    {
        if(execlp("srcml", "srcml", "--position", srcPath, "-o", desPath, NULL) < 0)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "convert %s to XML failed: %s.\n", srcPath, strerror(errno));
            RecordLog(error_info);

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
            RecordLog(error_info);
            
            return false;
        }
        else if(status == 0)   //srcml execute success return 0
            return true;
        else
            return false;
    }
}

static void *pthread_handle_Convert(void *arg)
{
    int *ret = malloc(sizeof(int));
    convertSrcPthread_arg *argument = (convertSrcPthread_arg *)arg;
    if(CodeToXML(argument->src_dir, argument->des_dir))
        *ret = 1;
    else
        *ret = 0;
    pthread_exit((void *)ret);
}

static void *pthread_handle_Analyze_C(void *arg)
{
    int *ret = malloc(sizeof(int));
    analyXmlPthread_arg *argument = (analyXmlPthread_arg *)arg;
    *ret = ExtractFuncFromCXML(argument->src_dir, tempFuncScoreTableName[argument->pthreadID], tempFuncCallTableName[argument->pthreadID]);
    pthread_exit((void *)ret);
}

static void *pthread_handle_Analyze_CPP(void *arg)
{
    int *ret = malloc(sizeof(int));
    analyXmlPthread_arg *argument = (analyXmlPthread_arg *)arg;
    *ret = ExtractFuncFromCPPXML(argument->src_dir, tempFuncScoreTableName[argument->pthreadID], tempFuncCallTableName[argument->pthreadID]);
    pthread_exit((void *)ret);
}

bool convertProgram(char *dirPath)
{
    char temp_dir[DIRPATH_MAX];
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
    createDir(temp_dir);
    
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
                RecordLog(error_info);
                closedir(pdir);
                
                return false;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                if(convertProgram(child_dir))
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
                if(judgeCPreprocessFile(child_dir) || judgeCPPPreprocessFile(child_dir))
                {
                    //handle C language preprocess file
                    curConvertSrcFileNum++;
                    printf("convert file %s(%d/%d)\n", child_dir, curConvertSrcFileNum, totalConvertSrcFileNum);
                    memset(xml_dir, 0, DIRPATH_MAX);
                    sprintf(xml_dir, "%s/%s.xml", temp_dir, pdirent->d_name);
                    
                    void *pthread_ret = NULL;
                    if(ConvertSRCPthreadRet[currentConvertSrcPthreadID] == 0)
                        pthread_join(convertSRCPthreadID[currentConvertSrcPthreadID], &pthread_ret);
                    memset(&convSrcPthreadArg[currentConvertSrcPthreadID], 0, sizeof(convertSrcPthread_arg));
                    strcpy(convSrcPthreadArg[currentConvertSrcPthreadID].des_dir, xml_dir);
                    strcpy(convSrcPthreadArg[currentConvertSrcPthreadID].src_dir, child_dir);
                    convSrcPthreadArg[currentConvertSrcPthreadID].pthreadID = 0;
                    ConvertSRCPthreadRet[currentConvertSrcPthreadID] = pthread_create(&convertSRCPthreadID[currentConvertSrcPthreadID], \
                    NULL, pthread_handle_Convert, (void *)&convSrcPthreadArg[currentConvertSrcPthreadID]);
                    
                    currentConvertSrcPthreadID++;
                    currentConvertSrcPthreadID = currentConvertSrcPthreadID%MAX_CONVERT_SRC_PTHREAD_NUM;
                    
                    if(pthread_ret != NULL)
                    {
                        ret = *(int *)pthread_ret;
                        free(pthread_ret);
                        if(!ret)
                        {
                            closedir(pdir);
                            return ret;
                        }
                    }
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", dirPath, strerror(errno));
        RecordLog(error_info);
        
        ret = false;
    }
    closedir(pdir);
    
    return ret;
}

bool analyzeProgram(char *dirPath)
{
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    
    char child_dir[DIRPATH_MAX];
    pdir = opendir(dirPath);
    bool ret = true;
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
                RecordLog(error_info);
                closedir(pdir);
                
                return ret;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                if(analyzeProgram(child_dir))
                    ret = true;
                else
                {
                    ret = false;
                    closedir(pdir);
                    return ret;
                }
            }
            else if(S_ISREG(statbuf.st_mode))
            {
                if(judgeCSrcXmlFile(child_dir))
                {
                    curAnalyzeXmlFileNum++;
                    printf("analysing file %s(%d/%d)\n", child_dir, curAnalyzeXmlFileNum, totalAnalyzeXmlFileNum);
                    
                    void *pthread_ret = NULL;
                    if(analyzeXMLPthreadRet[currentAnalyzeXmlPthreadID] == 0)
                        pthread_join(analyzeXMLPthreadID[currentAnalyzeXmlPthreadID], &pthread_ret);
                    memset(&analyXmlPthreadArg[currentAnalyzeXmlPthreadID], 0, sizeof(analyXmlPthread_arg));
                    strcpy(analyXmlPthreadArg[currentAnalyzeXmlPthreadID].src_dir, child_dir);
                    analyXmlPthreadArg[currentAnalyzeXmlPthreadID].pthreadID = currentAnalyzeXmlPthreadID;
                    analyzeXMLPthreadRet[currentAnalyzeXmlPthreadID] = pthread_create(&analyzeXMLPthreadID[currentAnalyzeXmlPthreadID],\
                    NULL, pthread_handle_Analyze_C, (void *)&analyXmlPthreadArg[currentAnalyzeXmlPthreadID]);
                   
                    currentAnalyzeXmlPthreadID++;
                    currentAnalyzeXmlPthreadID = currentAnalyzeXmlPthreadID%MAX_ANALYZE_XML_PTHREAD_NUM;
                    
                    if(pthread_ret != NULL)
                    {
                        ret = *(int *)pthread_ret;
                        free(pthread_ret);
                        if(!ret)
                        {
                            closedir(pdir);
                            return ret;
                        }
                    }
                }
                else if(judgeCPPSrcXmlFile(child_dir))
                {
                    //handle C++ language preprocess file
                    curAnalyzeXmlFileNum++;
                    printf("analysing file %s(%d/%d)\n", child_dir, curAnalyzeXmlFileNum, totalAnalyzeXmlFileNum);
                    
                    void *pthread_ret = NULL;
                    if(analyzeXMLPthreadRet[currentAnalyzeXmlPthreadID] == 0)
                        pthread_join(analyzeXMLPthreadID[currentAnalyzeXmlPthreadID], &pthread_ret);
                    memset(&analyXmlPthreadArg[currentAnalyzeXmlPthreadID], 0, sizeof(analyXmlPthread_arg));
                    strcpy(analyXmlPthreadArg[currentAnalyzeXmlPthreadID].src_dir, child_dir);
                    analyXmlPthreadArg[currentAnalyzeXmlPthreadID].pthreadID = currentAnalyzeXmlPthreadID;
                    analyzeXMLPthreadRet[currentAnalyzeXmlPthreadID] = pthread_create(&analyzeXMLPthreadID[currentAnalyzeXmlPthreadID],\
                    NULL, pthread_handle_Analyze_CPP, (void *)&analyXmlPthreadArg[currentAnalyzeXmlPthreadID]);
                   
                    currentAnalyzeXmlPthreadID++;
                    currentAnalyzeXmlPthreadID = currentAnalyzeXmlPthreadID%MAX_ANALYZE_XML_PTHREAD_NUM;
                    
                    if(pthread_ret != NULL)
                    {
                        ret = *(int *)pthread_ret;
                        free(pthread_ret);
                        if(!ret)
                        {
                            closedir(pdir);
                            return ret;
                        }
                    }
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", dirPath, strerror(errno));
        RecordLog(error_info);
    }
    closedir(pdir);
    
    return ret;
}

int getTotalAnalyzeFileNum(char *dirPath)
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
                RecordLog(error_info);
                closedir(pdir);
                
                return 0;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                ret += getTotalAnalyzeFileNum(child_dir);
            }
            if(S_ISREG(statbuf.st_mode))
            {
                if(judgeCPreprocessFile(child_dir))
                {
                    ret++;
                }
                else if(judgeCPPPreprocessFile(child_dir))
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
        RecordLog(error_info);
    }
    closedir(pdir);
    
    return ret;
}

bool initSoftware(char *srcPath)
{
    bool ret = getProgramName(srcPath);
    ret = deleteTempXMLFile();
    ret = buildLibrary();
    ret = buildTempTable();
    totalAnalyzeXmlFileNum = totalConvertSrcFileNum = getTotalAnalyzeFileNum(srcPath);
    
    int i;
    for(i = 0; i < MAX_CONVERT_SRC_PTHREAD_NUM; i++)
        ConvertSRCPthreadRet[i] = -1;
    curConvertSrcFileNum = 0;
    currentConvertSrcPthreadID = 0;
    ret = convertProgram(srcPath);
    for(i = 0; i < MAX_CONVERT_SRC_PTHREAD_NUM; i++)
    {
        if(ConvertSRCPthreadRet[i] == 0)
            pthread_join(convertSRCPthreadID[i], NULL);
    }
    
    char temp_dir[DIRPATH_MAX] = "";
    sprintf(temp_dir, "temp_%s", programName);
    for(i = 0; i < MAX_ANALYZE_XML_PTHREAD_NUM; i++)
        analyzeXMLPthreadRet[i] = -1;
    curAnalyzeXmlFileNum = 0;
    currentAnalyzeXmlPthreadID = 0;
    ret = analyzeProgram(temp_dir);
    for(i = 0; i < MAX_ANALYZE_XML_PTHREAD_NUM; i++)
    {
        if(analyzeXMLPthreadRet[i] == 0)
            pthread_join(analyzeXMLPthreadID[i], NULL);
    }
    ret = buildFuncScore();
    
    return ret;
}

void optDataBaseOper(char *tempFuncScoreTableName, char *tempFuncCallTableName)
{
    //create temp connect
    MYSQL temp_db;
    MYSQL *tempMysqlConnect = NULL;
    tempMysqlConnect = mysql_init(&temp_db);
    if(tempMysqlConnect == NULL)
    {
        RecordLog("init mysql failure\n");
        return ;
    }
    if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
        RecordLog(error_info);
        return ;
    }
    char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
    sprintf(tempSqlCommand, "delete from %s where funcName not in (select calledFunc from %s) and type='static'", tempFuncScoreTableName, tempFuncCallTableName);
    if(!executeSQLCommand(tempMysqlConnect, tempSqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
        RecordLog(error_info);
    }
    //merge tempFuncScore into funcScore
    memset(tempSqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(tempSqlCommand, "insert into %s select distinct * from %s", funcScoreTableName, tempFuncScoreTableName);
    if(!executeSQLCommand(tempMysqlConnect, tempSqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
        RecordLog(error_info);
    }
    // delete library function call record from funcCall
    memset(tempSqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(tempSqlCommand, "delete from %s where funcName not in (select funcName from %s) and funcCallType='static'", tempFuncCallTableName, tempFuncScoreTableName);
    if(!executeSQLCommand(tempMysqlConnect, tempSqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
        RecordLog(error_info);
    }
    //merge tempFuncCall into funcCall
    memset(tempSqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(tempSqlCommand, "insert into %s select distinct * from %s", funcCallTableName, tempFuncCallTableName);
    if(!executeSQLCommand(tempMysqlConnect, tempSqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
        RecordLog(error_info);
    }
    //clear tempFuncScore
    memset(tempSqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(tempSqlCommand, "truncate table %s", tempFuncScoreTableName);
    if(!executeSQLCommand(tempMysqlConnect, tempSqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
        RecordLog(error_info);
    }
    //clear tempFuncCall
    memset(tempSqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(tempSqlCommand, "truncate table %s", tempFuncCallTableName);
    if(!executeSQLCommand(tempMysqlConnect, tempSqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", tempSqlCommand);
        RecordLog(error_info);
    }
    mysql_close(tempMysqlConnect);
}

bool deleteTempXMLFile()
{
    bool ret = false;
    //delete xml file
    char temp[MAX_PROGRAMNAME_NUM] = "temp_";
    strcat(temp, programName);
    if(access(temp, F_OK) == 0)
        ret = deleteDir(temp);
        
    return ret;
}

bool buildFuncScore()
{
    bool ret = false;
    printf("updating funcCall table\n");
    time_t start, end, finish; 
    time(&start);
    // delete library function call record from funcCall
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "delete from %s where funcName not in (select funcName from %s)", funcCallTableName, funcScoreTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    //update funcCall table type field
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "update %s as funcScore, %s as funcCall set funcCall.type='S' where funcScore.funcName=funcCall.calledFunc", funcScoreTableName, funcCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
        ret = true;

    time(&end); 
    finish = end - start;
    printf("time is: %d second\n", (int)finish);
    printf("update funcCall table finish\n");
    
    //对每个函数进行打分， 每个函数中直接使用到的库函数
    char tmp_table[MAX_PROGRAMNAME_NUM*2] = "";
    sprintf(tmp_table, "tmp_%s_table", programName);
    replaceChar(tmp_table, '-', '_');
    replaceChar(tmp_table, '.', '_');
    
    //防止程序中途退出，导致临时表没有被及时删除
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "drop table if exists %s", tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    //MEM score
    printf("MEM score calculating\n");
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "create table %s (select * from %s as funcCall where funcCall.calledFunc in\
            (select funcName from funcLibrary where type='MEM') and type='L')", tmp_table, funcCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
        sprintf(sqlCommand, "alter table %s add column score int", tmp_table);
        if(!executeCommand(sqlCommand))
        {
            RecordLog("execute commad alter table tmp_table add column score int failure.");
        }
        else
        {
            memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
            sprintf(sqlCommand, "update %s as tmp_table, funcLibrary set tmp_table.score=(tmp_table.forNum*%d+tmp_table.whileNum*%d+1)*funcLibrary.score\
            where tmp_table.calledFunc=funcLibrary.funcName", tmp_table, FORNUM, WHILENUM);
            if(!executeCommand(sqlCommand))
            {
                RecordLog("execute commad update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName failure.");
            }
        }
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "update %s as tmp_table, %s as funcScore set funcScore.MEM=(select sum(score) from %s as tmp_table where tmp_table.funcName=funcScore.funcName \
    and tmp_table.sourceFile=funcScore.sourceFile) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile", tmp_table, funcScoreTableName, tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "drop table %s", tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
        ret = true;
        
    //CPU score
    printf("CPU score calculating\n");
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);

    sprintf(sqlCommand, "create table %s (select * from %s as funcCall where funcCall.calledFunc in\
            (select funcName from funcLibrary where type='CPU') and type='L')", tmp_table, funcCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
        sprintf(sqlCommand, "alter table %s add column score int", tmp_table);
        if(!executeCommand(sqlCommand))
        {
            RecordLog("execute commad alter table tmp_table add column score int failure.");
        }
        else
        {
            memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
            sprintf(sqlCommand, "update %s as tmp_table, funcLibrary set tmp_table.score=(tmp_table.forNum*%d+tmp_table.whileNum*%d+1)*funcLibrary.score\
            where tmp_table.calledFunc=funcLibrary.funcName", tmp_table, FORNUM, WHILENUM);
            if(!executeCommand(sqlCommand))
            {
                RecordLog("execute commad update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName failure.");
            }
        }
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "update %s as tmp_table, %s as funcScore set funcScore.CPU=(select sum(score) from %s as tmp_table where tmp_table.funcName=funcScore.funcName \
    and tmp_table.sourceFile=funcScore.sourceFile) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile", tmp_table, funcScoreTableName, tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "drop table %s", tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
        ret = true;
        
    //NET score
    printf("NET score calculating\n");
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "create table %s (select * from %s as funcCall where funcCall.calledFunc in\
            (select funcName from funcLibrary where type='NET') and type='L')", tmp_table, funcCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
        sprintf(sqlCommand, "alter table %s add column score int", tmp_table);
        if(!executeCommand(sqlCommand))
        {
            RecordLog("execute commad alter table tmp_table add column score int failure.");
        }
        else
        {
            memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
            sprintf(sqlCommand, "update %s as tmp_table, funcLibrary set tmp_table.score=(tmp_table.forNum*%d+tmp_table.whileNum*%d+1)*funcLibrary.score\
            where tmp_table.calledFunc=funcLibrary.funcName", tmp_table, FORNUM, WHILENUM);
            if(!executeCommand(sqlCommand))
            {
                RecordLog("execute commad update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName failure.");
            }
        }
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "update %s as tmp_table, %s as funcScore set funcScore.NET=(select sum(score) from %s as tmp_table where tmp_table.funcName=funcScore.funcName \
    and tmp_table.sourceFile=funcScore.sourceFile) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile", tmp_table, funcScoreTableName, tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "drop table %s", tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
        ret = true;
        
    //IO score
    printf("IO score calculating\n");
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "create table %s (select * from %s as funcCall where funcCall.calledFunc in\
            (select funcName from funcLibrary where type='IO') and type='L')", tmp_table, funcCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
        sprintf(sqlCommand, "alter table %s add column score int", tmp_table);
        if(!executeCommand(sqlCommand))
        {
            RecordLog("execute commad alter table tmp_table add column score int failure.");
        }
        else
        {
            memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
            sprintf(sqlCommand, "update %s as tmp_table, funcLibrary set tmp_table.score=(tmp_table.forNum*%d+tmp_table.whileNum*%d+1)*funcLibrary.score\
            where tmp_table.calledFunc=funcLibrary.funcName", tmp_table, FORNUM, WHILENUM);
            if(!executeCommand(sqlCommand))
            {
                RecordLog("execute commad update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName failure.");
            }
        }
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "update %s as tmp_table, %s as funcScore set funcScore.IO=(select sum(score) from %s as tmp_table where tmp_table.funcName=funcScore.funcName \
    and tmp_table.sourceFile=funcScore.sourceFile) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile", tmp_table, funcScoreTableName, tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "drop table %s", tmp_table);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
        ret = true;
    
    //delete funcCall table Library function record
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "delete from %s where type='L'", funcCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
        ret = true;
    
    //set funcCall calledFunctype
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "update %s as funcScore, %s as funcCall set funcCall.calledFuncType=funcScore.type \
    where funcScore.sourceFile=funcCall.sourceFile and funcCall.calledFunc=funcScore.funcName", funcScoreTableName, funcCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
        ret = true;
    
    //set funcCall CalledSrcFile
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "update %s as funcScore, %s as funcCall set funcCall.CalledSrcFile=funcScore.sourceFile \
    where funcCall.calledFuncType='extern' and funcCall.calledFunc=funcScore.funcName", funcScoreTableName, funcCallTableName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
        ret = true;
        
    return ret;
}

void getVarUsedFunc(char *varName, char *xmlPath)
{
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    
    char child_dir[DIRPATH_MAX];
    pdir = opendir(xmlPath);
    if(pdir)
    {
        while((pdirent = readdir(pdir)) != NULL)
        {
            //跳过"."和".."和隐藏文件夹
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 || (pdirent->d_name[0] == '.'))
                continue;
            
            memset(child_dir, 0, DIRPATH_MAX);
            sprintf(child_dir, "%s/%s", xmlPath, pdirent->d_name);
            if(lstat(child_dir, &statbuf) < 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "lstat %s to failed: %s.\n", child_dir, strerror(errno));
                RecordLog(error_info);
                closedir(pdir);
                
                return ;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                getVarUsedFunc(varName, child_dir);
            }
            if(S_ISREG(statbuf.st_mode))
            {
                memset(xml_dir, 0, DIRPATH_MAX);
                sprintf(xml_dir, "%s/%s", xmlPath, pdirent->d_name);
                if(judgeCSrcXmlFile(xml_dir))
                {
                    funcList * ret = ExtractVarUsedFunc(varName, xml_dir);
                    if(ret != NULL)
                    {
                        funcList *current = ret;
                        while(current != NULL)
                        {
                            memset(src_dir, 0, DIRPATH_MAX);
                            //删除开头的temp_和结尾的.xml
                            strncpy(src_dir, (char *)&(xml_dir[5]), strlen(xml_dir)-9);
                            printf("%s: %s(%d)\n", src_dir, current->funcName, current->line);
                            current = current->next;
                        }
                        //delete funcList value
                        current = ret;
                        while(current != NULL)
                        {
                            ret = ret->next;
                            free(current);
                            current = ret;
                        }
                    }
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", xmlPath, strerror(errno));
        RecordLog(error_info);
    }
    closedir(pdir);
}

confScore getFuncScore(char *funcName, bool funcType, char *argumentType, char *srcFile, int curPthreadID)
{
    funcCallCount[curPthreadID]++;
    confScore ret;
    memset(&ret, 0, sizeof(confScore));
    int argumentNum = getSpecCharNumFromStr(argumentType, '/') + 1;
    char selectArgumentType[512] = "(";
    if(argumentNum == 0)
    {
        //only one argument
        if(strstr(argumentType, "non") == NULL)
            strcpy(selectArgumentType, argumentType);
        else
            strcpy(selectArgumentType, "(%#1)");
    }
    else
    {
        //more than one argument
        char (*arguType)[MAX_SUBSTR] = (char (*)[MAX_SUBSTR])malloc(argumentNum*MAX_SUBSTR);
        int arguNum = 0;
        removeChar(argumentType, '(');
        removeChar(argumentType, ')');
        removeChar(argumentType, '#');
        removeNum(argumentType);
        cutStrByLabel(argumentType, '/', arguType, argumentNum);
        
        for(arguNum = 0; arguNum < argumentNum; arguNum++)
        {
            if(arguNum != 0)
                strcat(selectArgumentType, "/");
            if(strcasecmp(arguType[arguNum], "non") == 0)
                strcat(selectArgumentType, "%");
            else
                strcat(selectArgumentType, arguType[arguNum]);
        }
        free(arguType);
        sprintf(selectArgumentType, "%s#%d)", selectArgumentType, argumentNum);
    }

    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    //一个程序中只能有一个extern函数
    //一个程序中可能会存在多个名称相同的static函数
    //一个源文件中不可能存在多个名称相同的static函数
    char tempSqlCommand[LINE_CHAR_MAX_NUM] = "";
    if(funcType)
        sprintf(tempSqlCommand, "select calledFunc, calledFuncType, CalledSrcFile, forNum, whileNum, calledFuncArgumentType from %s where \
        funcName='%s' and funcCallType='static' and sourceFile='%s' and argumentType like '%s'", funcCallTableName, funcName, srcFile, selectArgumentType);
    else
        sprintf(tempSqlCommand, "select calledFunc, calledFuncType, CalledSrcFile, forNum, whileNum, calledFuncArgumentType from %s where \
        funcName='%s' and funcCallType='extern' and sourceFile='%s' and argumentType like '%s'", funcCallTableName, funcName, srcFile, selectArgumentType);
    MYSQL temp_db;
    MYSQL *tempMysqlConnect = NULL;
    tempMysqlConnect = mysql_init(&temp_db);
    if(tempMysqlConnect == NULL)
    {
        RecordLog("init mysql failure\n");
        return ret;
    }
    if(NULL == mysql_real_connect((MYSQL *)tempMysqlConnect, bind_address, user, pass, database, port, NULL, 0))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "connect failed: %s\n", mysql_error(tempMysqlConnect));
        RecordLog(error_info);
        mysql_close(tempMysqlConnect);
        return ret;
    }
    if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
        RecordLog(error_info);
        mysql_close(tempMysqlConnect);
        return ret;
    }
    else
    {
        MYSQL_RES *res_ptr1, *res_ptr2;
        MYSQL_ROW sqlrow1, sqlrow2;
        res_ptr1 = mysql_store_result(tempMysqlConnect);
        int rownum = mysql_num_rows(res_ptr1);
        //funcCallCount[curPthreadID] 为函数调用的最大深度
        if(rownum != 0 && funcCallCount[curPthreadID] < max_funcCallRecursive_NUM)
        {
            while(sqlrow1 = mysql_fetch_row(res_ptr1))
            {
                int multiple = StrToInt(sqlrow1[3])*FORNUM + StrToInt(sqlrow1[4])*WHILENUM + 1;
                bool temp_funcType = false;
                if(strcasecmp(sqlrow1[1], "static") == 0)
                {
                    temp_funcType = true;
                }
#if DEBUG == 1                
                printf("(%s(%s)->%s(%s))\n", funcName, argumentType, sqlrow1[0], sqlrow1[5]);
#endif                
                confScore temp_ret = getFuncScore(sqlrow1[0], temp_funcType, sqlrow1[5], sqlrow1[2], curPthreadID);
                ret.CPU += (temp_ret.CPU*multiple);
                ret.MEM += (temp_ret.MEM*multiple);
                ret.IO += (temp_ret.IO*multiple);
                ret.NET += (temp_ret.NET*multiple);
            }
            mysql_free_result(res_ptr1);
        }
        //get function score from funcScore table
        memset(tempSqlCommand, 0, LINE_CHAR_MAX_NUM);
        if(funcType)
            sprintf(tempSqlCommand, "select CPU, MEM, IO, NET from %s where \
            funcName='%s' and type='static' and sourceFile='%s' and argumentType like '%s'", funcScoreTableName, funcName, srcFile, selectArgumentType);
        else
            sprintf(tempSqlCommand, "select CPU, MEM, IO, NET from %s where \
            funcName='%s' and type='extern' and sourceFile='%s' and argumentType like '%s'", funcScoreTableName, funcName, srcFile, selectArgumentType);
        if(mysql_real_query(tempMysqlConnect, tempSqlCommand, strlen(tempSqlCommand)) != 0)
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "execute command failed: %s\n", mysql_error(tempMysqlConnect));
            RecordLog(error_info);
            mysql_close(tempMysqlConnect);
            return ret;
        }
        else
        {
            res_ptr2 = mysql_store_result(tempMysqlConnect);
            rownum = mysql_num_rows(res_ptr2);
            if(rownum != 0)
            {
                while(sqlrow2 = mysql_fetch_row(res_ptr2))
                {
                    ret.CPU += StrToInt(sqlrow2[0]);
                    ret.MEM += StrToInt(sqlrow2[1]);
                    ret.IO += StrToInt(sqlrow2[2]);
                    ret.NET += StrToInt(sqlrow2[3]);
                }
                mysql_free_result(res_ptr2);
            }
        }
    }
    mysql_close(tempMysqlConnect);
    funcCallCount[curPthreadID]--;
    
    return ret;
}

static void *getScore(void *arg)
{
    confScore *ret = malloc(sizeof(confScore));
    memset(ret, 0, sizeof(confScore));
    analyConfOptPthread_arg *argument = (analyConfOptPthread_arg *)arg;
    funcList * ret_begin = NULL;
    if(judgeCSrcXmlFile(argument->xmlFilePath))
        ret_begin = CSclice(argument->confOptName, argument->xmlFilePath);
    else if(judgeCPPSrcXmlFile(argument->xmlFilePath))
        ret_begin = CPPSclice(argument->confOptName, argument->xmlFilePath);

    if(ret_begin != NULL)
    {
        funcList *current = ret_begin;
        while(current != NULL)
        {
            funcCallCount[argument->pthreadID] = 0;
            confScore temp_ret = getFuncScore(current->funcName, current->funcType, current->argumentType, current->sourceFile, argument->pthreadID);
            ret->CPU += temp_ret.CPU;
            ret->MEM += temp_ret.MEM;
            ret->IO += temp_ret.IO;
            ret->NET += temp_ret.NET;
            current = current->next;                        
        }
        //delete funcList value
        current = ret_begin;
        while(current != NULL)
        {
            ret_begin = ret_begin->next;
            free(current);
            current = ret_begin;
        }
    }
    
    pthread_exit((void*)ret);
}

confScore buildConfScore(char *confName, char *xmlPath)
{
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    confScore ret;
    memset(&ret, 0, sizeof(confScore));
    
    char child_dir[DIRPATH_MAX];
    pdir = opendir(xmlPath);
    if(pdir)
    {
        while((pdirent = readdir(pdir)) != NULL)
        {
            //跳过"."和".."和隐藏文件夹
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 || (pdirent->d_name[0] == '.'))
                continue;
            
            memset(child_dir, 0, DIRPATH_MAX);
            sprintf(child_dir, "%s/%s", xmlPath, pdirent->d_name);
            if(lstat(child_dir, &statbuf) < 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "lstat %s to failed: %s.\n", child_dir, strerror(errno));
                RecordLog(error_info);
                closedir(pdir);
                
                return ret;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                confScore temp_ret = buildConfScore(confName, child_dir);
                ret.CPU += temp_ret.CPU;
                ret.MEM += temp_ret.MEM;
                ret.IO += temp_ret.IO;
                ret.NET += temp_ret.NET;
            }
            if(S_ISREG(statbuf.st_mode))
            {
                void *pthread_ret = NULL;
                if(analyzeConfOptPthreadRet[currentAnalyzeConfOptPthreadID] == 0)
                    pthread_join(analyzeConfOptPthreadID[currentAnalyzeConfOptPthreadID], &pthread_ret);
                memset(&analyConfOptPthreadArg[currentAnalyzeConfOptPthreadID], 0, sizeof(analyConfOptPthread_arg));
                strcpy(analyConfOptPthreadArg[currentAnalyzeConfOptPthreadID].xmlFilePath, child_dir);
                strcpy(analyConfOptPthreadArg[currentAnalyzeConfOptPthreadID].confOptName, confName);
                analyConfOptPthreadArg[currentAnalyzeConfOptPthreadID].pthreadID = currentAnalyzeConfOptPthreadID;
                analyzeConfOptPthreadRet[currentAnalyzeConfOptPthreadID] = pthread_create(&analyzeConfOptPthreadID[currentAnalyzeConfOptPthreadID], \
                NULL, getScore, (void *)&analyConfOptPthreadArg[currentAnalyzeConfOptPthreadID]);
               
                currentAnalyzeConfOptPthreadID++;
                currentAnalyzeConfOptPthreadID = currentAnalyzeConfOptPthreadID%MAX_ANALYZE_CONFOPT_PTHREAD_NUM;
                
                if(pthread_ret != NULL)
                {
                    confScore *temp_ret = (confScore *)pthread_ret;
                    ret.CPU += temp_ret->CPU;
                    ret.MEM += temp_ret->MEM;
                    ret.IO += temp_ret->IO;
                    ret.NET += temp_ret->NET;
                    free(pthread_ret);
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", xmlPath, strerror(errno));
        RecordLog(error_info);
    }
    closedir(pdir);
    
    return ret;
}

void getConfKeyInfluence(char *confKeyName, char *dirPath)
{
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    
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
                RecordLog(error_info);
                closedir(pdir);
                
                return ;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                getConfKeyInfluence(confKeyName, child_dir);
            }
            else if(S_ISREG(statbuf.st_mode))
            {
                if(ScliceConfKey(confKeyName, child_dir))
                    printf("-----file Name = %s-----\n", child_dir);
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", dirPath, strerror(errno));
        RecordLog(error_info);
    }
    closedir(pdir);
}