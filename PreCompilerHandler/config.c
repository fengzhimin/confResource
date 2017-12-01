/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : config.c
* Descripe     : global variable
******************************************************/

#include "config.h"

pthread_t exeCPreCompilePthreadID[MAX_EXECPRECOMPILE_PTHREAD_NUM];
int exeCPreCompilePthreadRet[MAX_EXECPRECOMPILE_PTHREAD_NUM] = { -1 };
int totalPreCompileFileNum = 0;
int curPreCompileFileNum = 0;
int currentPreCompilePthreadID = 0;
char shellPath[MAX_EXECPRECOMPILE_PTHREAD_NUM][DIRPATH_MAX] = { "" };
char JsonPath[DIRPATH_MAX] = "";