/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : searchLoopCount.h
* Descripe     : search loop count
******************************************************/

#ifndef __SEARCHLOOPCOUNT_H__
#define __SEARCHLOOPCOUNT_H__

#include "logOper.h"
#include "dirOper.h"
#include "fileOper.h"
#include "config.h"
#include "strOper.h"

/*******************************************
 * func: 匹配该文件是否为记录循环次数的文件
 * return: true = yes   false = no
 * @para fileName1: 要被判断的文件名
 * @para fileName2: 需要查找的文件名
*******************************************/
bool JudgeRecordLoopCountFile(char *fileName1, char *fileName2);

/********************************************
 * func: 统计一个文件中要查询循环的次数
 * return: 返回loopRecordInfo结构体
 * @para recordCountFilePath: 文件路径
 * @para: loopInfo: 要查询的循环信息
*********************************************/
loopRecordInfo getLoopRecordFromFile(char *recordCountFilePath, loopCountInfo loopInfo);

/*******************************************
 * func: 递归从record_count中查找匹配的文件名
 * return: 返回loopRecordInfo结构体
 * @para recordCountPath: 存放记录循环次数的文件夹
 * @para: loopInfo: 要查询的循环信息
********************************************/
loopRecordInfo getLoopRecord(char *recordCountPath, loopCountInfo loopInfo);

/*******************************************
 * func: 统计出要查找循环的次数
 * return: 计算出的平均循环次数
 * @para: loopInfo: 要查询的循环信息
********************************************/
int getLoopCount(loopCountInfo loopInfo);

#endif