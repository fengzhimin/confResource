/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : searchLoopCount.c
* Descripe     : search loop count
******************************************************/

#include "searchLoopCount.h"

static char error_info[LOGINFO_LENGTH];
static char lineData[LINE_CHAR_MAX_NUM];

bool JudgeRecordLoopCountFile(char *fileName1, char *fileName2)
{
    char temp_fileName1[DIRPATH_MAX] = "";
    char temp_fileName2[DIRPATH_MAX] = "";
    strncpy(temp_fileName1, fileName1, ExtractLastCharIndex(fileName1, '.'));
    strcpy(temp_fileName2, &(fileName2[ExtractLastCharIndex(fileName2, '/')+1]));
    char fileType[5] = "";
    int temp_index = ExtractLastCharIndex(temp_fileName1, '.');
    strcpy(fileType, &(temp_fileName1[temp_index]));
    sprintf(&(temp_fileName1[temp_index]), ".E%s", fileType);
    
    if(strcasecmp(temp_fileName1, temp_fileName2) == 0)
        return true;
    else
        return false;
}

loopRecordInfo getLoopRecordFromFile(char *recordCountFilePath, loopCountInfo loopInfo)
{
    loopRecordInfo ret;
    memset(&ret, 0, sizeof(loopRecordInfo));
    int fd = OpenFile(recordCountFilePath, O_RDONLY);
	if(fd == -1)
	{
		memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open file %s failed: %s.\n", recordCountFilePath, strerror(errno));
        Error(error_info);
        
        return ret;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	while(ReadLine(fd, lineData) == -1)
	{
		if(strstr(lineData, loopInfo.funcName) != NULL)
        {
            char strCount[MAX_VARIABLE_LENGTH] = "";
            sprintf(strCount, "count%d", loopInfo.count);
            if(strstr(lineData, strCount) != NULL)
            {
                ret.recordNum++;
                ret.sum += StrToInt(&(lineData[ExtractLastCharIndex(lineData, '=')+1]));
            }
        }

		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	CloseFile(fd);
	return ret;
}

loopRecordInfo getLoopRecord(char *recordCountPath, loopCountInfo loopInfo)
{
    DIR *pdir;
    struct dirent *pdirent;
    struct stat statbuf;
    loopRecordInfo ret;
    memset(&ret, 0, sizeof(loopRecordInfo));
    
    char child_dir[DIRPATH_MAX];
    pdir = opendir(recordCountPath);
    if(pdir)
    {
        while((pdirent = readdir(pdir)) != NULL)
        {
            //跳过"."和".."和隐藏文件夹
            if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 || (pdirent->d_name[0] == '.'))
                continue;
            
            memset(child_dir, 0, DIRPATH_MAX);
            sprintf(child_dir, "%s/%s", recordCountPath, pdirent->d_name);
            if(lstat(child_dir, &statbuf) < 0)
            {
                memset(error_info, 0, LOGINFO_LENGTH);
                sprintf(error_info, "lstat %s to failed: %s.\n", child_dir, strerror(errno));
                Error(error_info);
                closedir(pdir);
                
                return ret;
            }
            
            //judge whether directory or not
            if(S_ISDIR(statbuf.st_mode))
            {
                loopRecordInfo temp_ret = getLoopRecord(child_dir, loopInfo);
                ret.recordNum += temp_ret.recordNum;
                ret.sum += temp_ret.sum;
            }
            else if(S_ISREG(statbuf.st_mode))
            {
                if(JudgeRecordLoopCountFile(pdirent->d_name, loopInfo.sourcePath))
                {
                    loopRecordInfo temp_ret = getLoopRecordFromFile(child_dir, loopInfo);
                    ret.recordNum += temp_ret.recordNum;
                    ret.sum += temp_ret.sum;
                }
            }
        }
    }
    else
    {
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open directory %s to failed: %s.\n", recordCountPath, strerror(errno));
        Error(error_info);
    }
    
    closedir(pdir);
    
    return ret;
}

int getLoopCount(loopCountInfo loopInfo)
{
    int ret = 1;
    loopRecordInfo temp_ret = getLoopRecord(recordCountPath, loopInfo);
    if(temp_ret.recordNum != 0)
    {
        ret = temp_ret.sum / temp_ret.recordNum;
        ret = (ret == 0)? 1 : ret;
    }
    
    return ret;
}