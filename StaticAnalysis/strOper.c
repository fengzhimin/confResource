/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : strOper.c
* Descripe     : common string operation
******************************************************/

#include "strOper.h"

void removeChar(char *str, char ch)
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
		if(temp[i] == ch)
			continue;
		else
			str[j++] = temp[i];
	}

	free(temp);
}

void removeNum(char *str)
{
    int _length = strlen(str);
	char *temp = malloc(sizeof(char)*(_length+1));
	memset(temp, 0, _length);
	strcpy(temp, str);
	memset(str, 0, _length);
	int j = 0;
	int i;
    int numChar = 0;
	for(i = 0; i < _length; i++)
	{
        numChar = temp[i]-'0';
		if(numChar <= 9 && numChar >= 0)
			continue;
		else
			str[j++] = temp[i];
	}

	free(temp);
}

void removeBeginSpace(char *str)
{
	int _length = strlen(str);
	char *temp = malloc(sizeof(char)*(_length+1));
	memset(temp, 0, _length);
	strcpy(temp, str);
	memset(str, 0, _length);
	char *pstr = &temp[0];
	int i;
	for(i = 0; i < _length; i++)
	{
		if(temp[i] == ' ' || temp[i] == '\t')
		{
			pstr = &temp[i+1];
			continue;
		}
		else
			break;
	}
	strcpy(str, pstr);
	free(temp);
}

int getSpecCharNumFromStr(char *str, char ch)
{
    int str_length = strlen(str);
    int char_num = 0;
    int i;
    for( i = 0; i < str_length; i++)
    {
        if(str[i] == ch)
            char_num++;
    }
    
    return char_num;
}

int cutStrByLabel(char *str, char ch, char subStr[][MAX_SUBSTR], int subStrLength)
{
	//将subStr清空
	int i;
	for(i = 0; i < subStrLength; i++)
		memset(subStr[i], 0, MAX_SUBSTR);

	int _strLength = strlen(str);
	char *pstr = &str[0];
	int _ret_subNum = 0;
	int j = 0;   //为上一个子字符串的最后一个字符的index+1
	for(i = 0; i < _strLength; i++)
	{
		if(str[i] == ch)
		{
			if((i-j-1) >= MAX_SUBSTR)
			{
				strncpy(subStr[_ret_subNum], pstr, MAX_SUBSTR-1);
				_ret_subNum++;
				if(subStrLength == (_ret_subNum+1))    //判断要截取的子串个数是否小于存放子串的数组大小
				{
					j = i + 1;
					pstr = &str[j];
					break;
				}
			}
			else if(i != j)   //如果是连续的ch分隔字符则跳过
			{
				strncpy(subStr[_ret_subNum], pstr, i-j);
				_ret_subNum++;
				if(subStrLength == (_ret_subNum+1))    //判断要截取的子串个数是否小于存放子串的数组大小
				{
					j = i + 1;
					pstr = &str[j];
					break;
				}
			}
			
			j = i + 1;
			pstr = &str[j];
		}
	}

	//将最后一部分字符串拷贝出来
	if(strlen(pstr) >= (MAX_SUBSTR+1))
	{
		strncpy(subStr[_ret_subNum], pstr, MAX_SUBSTR-1);
	}
	else
		strcpy(subStr[_ret_subNum], pstr);

	return _ret_subNum+1;
}


char* IntToStr(char *str, int num)
{
	if(num < 10)
	{
		str[0] = num + '0';
		return ++str;
	}
	else
	{
		str = IntToStr(str, num/10);
		str[0] = num%10 + '0';
		return ++str;
	}
}

int StrToInt(char *str)
{
	int strLength = strlen(str);
	int i, retNum = 0;
	for(i = 0; i < strLength; i++)
	{
        if(str[i] < '0' || str[i] > '9')
            continue;
        
		retNum *= 10;
		retNum += str[i] - '0';
	}

	return retNum;
}

int ExtractLastCharIndex(char *str, char ch)
{
    int strLength = strlen(str);
    int i, ret = -1;
    for(i = strLength - 1; i >= 0; i--)
    {
        if(str[i] == ch)
            return i;
    }
    
    return ret;
}

char *ExtractStringFromLiteral(char *str)
{
    int strlength = 0;
    while(str[strlength] != '\0')
        strlength++;
    
    char *ret = malloc(sizeof(char)*(strlength-1));
    memset(ret, 0, sizeof(char)*(strlength-1));
    strncpy(ret, &str[1], strlength-2);
    
    return ret;
}

int replaceChar(char *str, char ch1, char ch2)
{
    int ret = 0;
    int strlength = strlen(str);
    int i = 0;
    for( ; i < strlength; i++)
    {
        if(str[i] == ch1)
        {
            str[i] = ch2;
            ret++;
        }
    }
    
    return ret;
}