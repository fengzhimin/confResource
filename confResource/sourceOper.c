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
                if(judgeCPreprocessFile(child_dir))
                {
                    printf("analysing file %s\n", child_dir);
                    memset(xml_dir, 0, DIRPATH_MAX);
                    sprintf(xml_dir, "%s/%s.xml", temp_dir, pdirent->d_name);
                    if(CodeToXML(child_dir, xml_dir))
                    {
                        ret = true;
                        ExtractFuncFromCXML(xml_dir);
                    }
                    else
                    {
                        ret = false;
                        closedir(pdir);
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

bool initSoftware(char *srcPath)
{
    bool ret = buildTempTable();
    ret = convertProgram(srcPath);
    ret = buildFuncScore();
    
    return ret;
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
    printf("time is: %d second\n", finish);
    printf("update funcCall table finish\n");
    
    //对每个函数进行打分， 每个函数中直接使用到的库函数
    //MEM score
    printf("MEM score calculating\n");
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "create table tmp_table (select * from funcCall where funcCall.calledFunc in\
            (select funcName from funcLibrary where type='MEM') and type='L')");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        if(!executeCommand("alter table tmp_table add column score int"))
        {
            RecordLog("execute commad alter table tmp_table add column score int failure.");
        }
        else
        {
            if(!executeCommand("update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName"))
            {
                RecordLog("execute commad update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName failure.");
            }
        }
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update tmp_table, funcScore set funcScore.MEM=(select sum(score) from tmp_table where tmp_table.funcName=funcScore.funcName \
    and tmp_table.sourceFile=funcScore.sourceFile) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile");
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
            (select funcName from funcLibrary where type='CPU') and type='L')");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        if(!executeCommand("alter table tmp_table add column score int"))
        {
            RecordLog("execute commad alter table tmp_table add column score int failure.");
        }
        else
        {
            if(!executeCommand("update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName"))
            {
                RecordLog("execute commad update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName failure.");
            }
        }
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update tmp_table, funcScore set funcScore.CPU=(select sum(score) from tmp_table where tmp_table.funcName=funcScore.funcName \
    and tmp_table.sourceFile=funcScore.sourceFile) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile");
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
            (select funcName from funcLibrary where type='NET') and type='L')");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        if(!executeCommand("alter table tmp_table add column score int"))
        {
            RecordLog("execute commad alter table tmp_table add column score int failure.");
        }
        else
        {
            if(!executeCommand("update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName"))
            {
                RecordLog("execute commad update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName failure.");
            }
        }
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update tmp_table, funcScore set funcScore.NET=(select sum(score) from tmp_table where tmp_table.funcName=funcScore.funcName \
    and tmp_table.sourceFile=funcScore.sourceFile) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile");
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
            (select funcName from funcLibrary where type='IO') and type='L')");
    if(!executeCommand(sqlCommand))
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
        RecordLog(error_info);
    }
    else
    {
        if(!executeCommand("alter table tmp_table add column score int"))
        {
            RecordLog("execute commad alter table tmp_table add column score int failure.");
        }
        else
        {
            if(!executeCommand("update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName"))
            {
                RecordLog("execute commad update tmp_table, funcLibrary set tmp_table.score=funcLibrary.score where tmp_table.calledFunc=funcLibrary.funcName failure.");
            }
        }
    }
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "update tmp_table, funcScore set funcScore.IO=(select sum(score) from tmp_table where tmp_table.funcName=funcScore.funcName \
    and tmp_table.sourceFile=funcScore.sourceFile) where tmp_table.funcName=funcScore.funcName and tmp_table.sourceFile=funcScore.sourceFile");
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
    
    //delete funcCall table Library function record
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    strcpy(sqlCommand, "delete from funcCall where type='L'");
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
    strcpy(sqlCommand, "update funcScore, funcCall set funcCall.calledFuncType=funcScore.type \
    where funcScore.sourceFile=funcCall.sourceFile and funcCall.calledFunc=funcScore.funcName");
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
    strcpy(sqlCommand, "update funcScore, funcCall set funcCall.CalledSrcFile=funcScore.sourceFile \
    where funcCall.calledFuncType='extern' and funcCall.calledFunc=funcScore.funcName");
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

confScore getFuncScore(char *funcName, bool funcType, char *srcFile)
{
    count++;
    confScore ret;
    memset(&ret, 0, sizeof(confScore));
    memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
    //一个程序中只能有一个external函数
    //一个程序中可能会存在多个名称相同的static函数
    //一个源文件中不可能存在多个名称相同的static函数
    if(funcType)
        sprintf(sqlCommand, "select calledFunc, calledFuncType, CalledSrcFile from funcCall where \
        funcName='%s' and funcCallType='static' and sourceFile='%s'", funcName, srcFile);
    else
        sprintf(sqlCommand, "select calledFunc, calledFuncType, CalledSrcFile from funcCall where \
        funcName='%s' and funcCallType='extern'", funcName);
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
        
        //count 为递归的最大深度
        if(rownum != 0 && count < max_funcCallRecursive_NUM)
        {
            while(sqlrow1 = mysql_fetch_row(res_ptr1))
            {
                bool temp_funcType = false;
                if(strcasecmp(sqlrow1[1], "static") == 0)
                {
                    temp_funcType = true;
                }
#if DEBUG == 1                
                printf("(%s->%s)", funcName, sqlrow1[0]);
#endif                
                confScore temp_ret = getFuncScore(sqlrow1[0], temp_funcType, sqlrow1[2]);
                ret.CPU += temp_ret.CPU;
                ret.MEM += temp_ret.MEM;
                ret.IO += temp_ret.IO;
                ret.NET += temp_ret.NET;
            }
            mysql_free_result(res_ptr1);
        }
#if DEBUG == 1
        else
            printf("\n");
#endif
        //get function score from funcScore table
        memset(sqlCommand, 0, LINE_CHAR_MAX_NUM);
        if(funcType)
            sprintf(sqlCommand, "select CPU, MEM, IO, NET from funcScore where \
            funcName='%s' and type='static'  and sourceFile='%s'", funcName, srcFile);
        else
            sprintf(sqlCommand, "select CPU, MEM, IO, NET from funcScore where \
            funcName='%s' and type='extern'", funcName);
        if(!executeCommand(sqlCommand))
        {
            memset(error_info, 0, LOGINFO_LENGTH);
            sprintf(error_info, "execute commad %s failure.\n", sqlCommand);
            RecordLog(error_info);
        }
        else
        {
            res_ptr2 = mysql_store_result(mysqlConnect);
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
                //跳过头文件
                //if(judgeCSrcXmlFile(child_dir))
                {
                    funcList * ret_begin = Sclice(confName, child_dir);
                    if(ret_begin != NULL)
                    {
                        funcList *current = ret_begin;
                        while(current != NULL)
                        {
#if DEBUG == 1                            
                            printf("sourceFile:%s funcName: %s funcType: %d\n", current->sourceFile, current->funcName, current->funcType);
#endif                        
                            confScore temp_ret = getFuncScore(current->funcName, current->funcType, current->sourceFile);
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

void getVarInfluence(char *var, char *dirPath)
{
    funcList *begin = NULL;
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
                
                return begin;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                getVarInfluence(var, child_dir);
            }
            else if(S_ISREG(statbuf.st_mode))
            {
                begin = Sclice(var, child_dir);
#if DEBUG == 1                
                if(begin != NULL)
                    printf("----------------file Name = %s---------------\n", child_dir);
#endif           
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
    
    return begin;
}