#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/file.h>

#define gettid() syscall(__NR_gettid)

void insert_count(char *fileName, char *srcPath, char *funcName, int countNum, int countValue)
{
	if(access("./record_count", F_OK) == -1)
	{
		//record_count文件夹不存在
		mkdir("./record_count", 0777);
	}
	char filePath[1024] = "";
	sprintf(filePath, "./record_count/%s", fileName);
	int fd = open(filePath, O_APPEND | O_RDWR | O_CREAT, 0644);
	int count = 0;
	while(fd == -1)
	{
		memset(filePath, 0, 1024);
		sprintf(filePath, "./record_count/%s%d", fileName, count++);
		fd = open(filePath, O_APPEND | O_RDWR | O_CREAT, 0644);
	}
	
	flock(fd, LOCK_EX);
	char str_countValue[10] = "";
	sprintf(str_countValue, "%d", countValue);
	char str_countNum[17] = "";
	sprintf(str_countNum, ":count%d=", countNum);
	write(fd, "srcPath=", 8);
	write(fd, srcPath, strlen(srcPath));
	write(fd, ":funcName=", 10);
	write(fd, funcName, strlen(funcName));
	write(fd, str_countNum, strlen(str_countNum));
	write(fd, str_countValue, strlen(str_countValue));
	write(fd, "\n", 1);
	
	close(fd);
}
