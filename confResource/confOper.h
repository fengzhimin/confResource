#ifndef __CONFOPER_H__
#define __CONFOPER_H__

#include "strOper.h"
#include "fileOper.h"
#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/***************************************
 * func: 读取配置项的值
 * return: true = 读取成功   false = 读取失败
 * @para label: 配置项所在的标签
 * @para key: 配置项的名称
 * @para value: 存放配置项的值
 * example: [label1]
 *			key1 = value1
 *			key2 = value2
 *			[label2]
 *			key = value
 *			getConfValueByLabelAndKey("label1", "key1", &value)   --->  value = value1
***************************************/
bool getConfValueByLabelAndKey(char *label, char *key, char *value);

#endif