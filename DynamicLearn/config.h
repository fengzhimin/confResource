/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : config.h
* Descripe     : the configuration of this code
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <iostream>
#include <string>

using namespace std;

//软件配置文件路径
#define SRC_CONF_OPT_PATH    "../SrcConfOpt.conf"

//使用OW算法生成测试案例
#define OW_SAMPLE_PATH       "OWSample.csv"

//使用nOW算法生成测试案例
#define NOW_SAMPLE_PATH      "nOWSample.csv"

//使用PW算法生成测试案例
#define PW_SAMPLE_PATH       "PWSample.csv"

//使用Rand Sample算法生成测试案例
#define RD_SAMPLE_PATH       "RDSAMPLE.csv"

//使用Plackett-Burman算法生成测试案例
#define PB_SAMPLE_PATH       "PBSAMPLE.csv"

//生成测试用例
#define SAMPLE_PATH          "SAMPLE.csv"

/************************
 * func: 定义软件待测配置项信息
 * 配置文件格式: confName confType minValue maxValue 开关型配置项minValue和maxValue的值为空
 * @para confName: 配置项名称
 * @para confType: 配置项类型　　0 表示开关型配置项　　　1　表示数值型配置项
 * @para minValue: 配置项最小值
 * @para maxValue: 配置项最大值
************************/
class SWConfInfo
{
public:
    string confName;
    short confType;
    int minValue;
    int maxValue;
public:
    SWConfInfo();
    ~SWConfInfo();
};

#endif