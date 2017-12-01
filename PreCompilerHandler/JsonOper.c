/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : JsonOper.c
* Descripe     : json file operation
******************************************************/

#include "JsonOper.h"

static char error_info[LOGINFO_LENGTH];
static char lineData[LINE_CHAR_MAX_NUM];
static char subStr2[2][MAX_SUBSTR];

static void removeJsonChar(char *str)
{
	int _length = strlen(str);
	char *temp = malloc(sizeof(char)*(_length+1));
	memset(temp, 0, _length);
	strcpy(temp, str);
	memset(str, 0, _length);
	int j = 0;
	int i;
	for(i = 0; i < _length; i++)
	{
		if(temp[i] == ' ' || temp[i] == '\"' || temp[i] == ',')
			continue;
		else
			str[j++] = temp[i];
	}

	free(temp);
}

bool getSoftWareConfInfo()
{
    int fd = OpenFile(INPUT_PATH, O_RDONLY);
	if(fd == -1)
	{
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open file(%s) failed: %s.\n", INPUT_PATH, strerror(errno));
        Error(error_info);
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
            //handle JsonPath and rebuild option
            cutStrByLabel(lineData, '=', subStr2, 2);
            if(strcasecmp(subStr2[0], "JsonPath") == 0)
            {
                //get JsonPath value
                strcpy(JsonPath, subStr2[1]);
                break;
            }
        }
next:
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	CloseFile(fd);
    
	return true;
}

PreCompileInfoList *ExtractInfoFromJson()
{
    PreCompileInfoList *begin = NULL;
    PreCompileInfoList *end = NULL;
    int fd = OpenFile(JsonPath, O_RDONLY);
    if(fd == -1)
	{
        memset(error_info, 0, LOGINFO_LENGTH);
        sprintf(error_info, "open file(%s) failed: %s.\n", JsonPath, strerror(errno));
        Error(error_info);
		return NULL;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	while(ReadLine(fd, lineData) == -1)
    {
        if(strstr(lineData, "\"arguments\"") != NULL)
        {
            //设置总的文件个数
            totalPreCompileFileNum++;
            
            if(begin == NULL)
                begin = end = malloc(sizeof(PreCompileInfoList));
            else
                end = end->next = malloc(sizeof(PreCompileInfoList));
                
            memset(end, 0, sizeof(PreCompileInfoList));
            
            memset(lineData, 0, LINE_CHAR_MAX_NUM);
            while(ReadLine(fd, lineData) == -1)
            {
                if(strstr(lineData, "]") != NULL)
                    break;
                if(strstr(lineData, "\"-o\"") != NULL)
                {
                    //输出文件
                    ReadLine(fd, lineData);
                }
                else if(strstr(lineData, "\"-c\"") != NULL)
                {
                    //设置预编译参数
                    strcat(end->info.preCompileCommand, " -E -P");
                }
                else
                {
                    removeJsonChar(lineData);
                    sprintf(end->info.preCompileCommand, "%s %s", end->info.preCompileCommand, lineData);
                    memset(lineData, 0, LINE_CHAR_MAX_NUM);
                }
                
                memset(lineData, 0, LINE_CHAR_MAX_NUM);
            }
        }
        else if(strstr(lineData, "\"directory\"") != NULL)
        {
            cutStrByLabel(lineData, ':', subStr2, 2);
            removeJsonChar(subStr2[1]);
            strcat(end->info.dirPath, subStr2[1]);
        }
        else if(strstr(lineData, "\"file\"") != NULL)
        {
            cutStrByLabel(lineData, ':', subStr2, 2);
            removeJsonChar(subStr2[1]);
            strcpy(end->info.fileName, subStr2[1]);
            //设置输入文件
            char temp_fileName[MAX_FILENAME_LENGTH] = "";
            int temp_position = ExtractLastCharIndex(subStr2[1], '.');
            strncpy(temp_fileName, subStr2[1], temp_position);
            sprintf(end->info.preCompileCommand, "%s -o %s.E%s", end->info.preCompileCommand, temp_fileName, &(subStr2[1][temp_position]));
        }
        
        memset(lineData, 0, LINE_CHAR_MAX_NUM);
    }
    
    return begin;
}