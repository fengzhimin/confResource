#include "sourceOper.h"

static char error_info[LOGINFO_LENGTH];
static char xml_dir[DIRPATH_MAX];
static char sqlCommand[LINE_CHAR_MAX_NUM];
static char src_dir[DIRPATH_MAX];
static int count = 0;

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

bool judgeCSrcFile(char *filePath)
{
    int index = ExtractLastCharIndex(filePath, '.');
    if(index != -1)
    {
        if(strcasecmp((char *)&filePath[index+1], "c") == 0 || strcasecmp((char *)&filePath[index+1], "cc") == 0)
            return true;
    }

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
                    return ret;
                }
            }
            if(S_ISREG(statbuf.st_mode))
            {
                if(judgeCSrcFile(child_dir))
                {
                    memset(xml_dir, 0, DIRPATH_MAX);
                    sprintf(xml_dir, "%s/%s.xml", temp_dir, pdirent->d_name);
                    if(CodeToXML(child_dir, xml_dir))
                    {
                        ret = true;
                        ExtractFuncFromXML(xml_dir);
                        printf("analysing file %s\n", child_dir);
                    }
                    else
                    {
                        ret = false;
                        return ret;
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

bool deleteTempXMLFile()
{
    char temp[MAX_PROGRAMNAME_NUM] = "temp_";
    strcat(temp, programName);
    if(access(temp, F_OK) == 0)
        return deleteDir(temp);
    else
        return true;
}

bool buildFuncScore()
{
    bool ret = false;
    printf("updating funcCall table\n");
    time_t start, end, finish; 
    time(&start); 
    //update funcCall table type field
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update funcScore, funcCall set funcCall.type='S' where funcScore.funcName=funcCall.calledFunc");
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
    printf("time is: %d\n", finish);
    printf("update funcCall table finish\n");
    //对每个函数进行打分， 每个函数中直接使用到的库函数
    //MEM score
    printf("MEM score calculating\n");
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "create table tmp_table (select * from funcCall where funcCall.calledFunc in\
            (select funcName from funcLibrary where type='MEM'))");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update tmp_table, funcScore set funcScore.MEM=(select count(*) from tmp_table\
            where tmp_table.funcName=funcScore.funcName) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "drop table tmp_table");
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
    strcpy(sqlCommand, "create table tmp_table (select * from funcCall where funcCall.calledFunc in \
            (select funcName from funcLibrary where type='CPU'))");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update tmp_table, funcScore set funcScore.CPU=(select count(*) from tmp_table \
            where tmp_table.funcName=funcScore.funcName) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "drop table tmp_table");
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
    strcpy(sqlCommand, "create table tmp_table (select * from funcCall where funcCall.calledFunc in \
            (select funcName from funcLibrary where type='NET'))");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update tmp_table, funcScore set funcScore.NET=(select count(*) from tmp_table \
            where tmp_table.funcName=funcScore.funcName) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "drop table tmp_table");
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
    strcpy(sqlCommand, "create table tmp_table (select * from funcCall where funcCall.calledFunc in \
            (select funcName from funcLibrary where type='IO'))");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update tmp_table, funcScore set funcScore.IO=(select count(*) from tmp_table \
            where tmp_table.funcName=funcScore.funcName) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "drop table tmp_table");
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
                funcList * ret = ExtractVarUsedPos(varName, xml_dir);
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
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", xmlPath, strerror(errno));
        RecordLog(error_info);
    }
    closedir(pdir);
}

confScore getFuncScore(char *funcName)
{
    count++;
    confScore ret;
    memset(&ret, 0, sizeof(confScore));
    
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    sprintf(sqlCommand, "select distinct calledFunc from funcCall where funcName='%s' and type='S'", funcName);
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        MYSQL_RES *res_ptr1, *res_ptr2;
        MYSQL_ROW sqlrow1, sqlrow2;
        res_ptr1 = mysql_store_result(mysqlConnect);
        int rownum = mysql_num_rows(res_ptr1);
        
        if(rownum != 0 && count < 8)
        {
            while(sqlrow1 = mysql_fetch_row(res_ptr1))
            {
                confScore temp_ret = getFuncScore(sqlrow1[0]);
                ret.CPU += temp_ret.CPU;
                ret.MEM += temp_ret.MEM;
                ret.IO += temp_ret.IO;
                ret.NET += temp_ret.NET;
            }
            mysql_free_result(res_ptr1);
        }
        //get function score from funcScore table
        memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
        sprintf(sqlCommand, "select distinct * from funcScore where funcName='%s'", funcName);
        if(!executeCommand(sqlCommand))
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
            RecordLog(error_info);
        }
        else
        {
            res_ptr2 = mysql_store_result(mysqlConnect);
            rownum = mysql_num_rows(res_ptr1);
            if(rownum != 0)
            {
                while(sqlrow2 = mysql_fetch_row(res_ptr2))
                {
                    ret.CPU += StrToInt(sqlrow2[3]);
                    ret.MEM += StrToInt(sqlrow2[4]);
                    ret.IO += StrToInt(sqlrow2[5]);
                    ret.NET += StrToInt(sqlrow2[6]);
                }
                mysql_free_result(res_ptr2);
            }
        }
    }
    count--;
    
    return ret;
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
                memset(xml_dir, 0, DIRPATH_MAX);
                sprintf(xml_dir, "%s/%s", xmlPath, pdirent->d_name);
                funcList * ret_begin = ExtractVarUsedPos(confName, xml_dir);
                if(ret_begin != NULL)
                {
                    funcList *current = ret_begin;
                    while(current != NULL)
                    {
                        confScore temp_ret = getFuncScore(current->funcName);
                        ret.CPU += temp_ret.CPU;
                        ret.MEM += temp_ret.MEM;
                        ret.IO += temp_ret.IO;
                        ret.NET += temp_ret.NET;
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
