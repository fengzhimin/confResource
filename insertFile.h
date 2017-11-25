/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : insertFile.h
* Descripe     : insert record loop count code
******************************************************/

#ifndef __INSERTFILE_H__
#define __INSERTFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************
 * func: 将循环结果插入文件
 * return: void
 * @para fileName: 文件名
 * @para srcPath: 当前循环所在文件的路径
 * @para funcName: 当前循环所在函数名
 * @para countNum: 当前循环变量的个数标记  例如 count2  --->   countNum = 2
 * @para countValue: 当前循环变量的值
********************************************/
void insert_count(char *fileName, char *srcPath, char *funcName, int countNum, int countValue);

#ifdef __cplusplus
}
#endif

#endif
