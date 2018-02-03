/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : SWConfOpt.h
* Descripe     : software configuration option
******************************************************/

#ifndef __SRC_CONF_OPT_H__
#define __SRC_CONF_OPT_H__

#include <string>
#include <vector>
#include "config.h"

using namespace std;

/************************************
 * func: 描述软件配置项
*************************************/
class ConfOpt
{
private:
    int binConfOptNum;          //开关型配置项个数
    int numConfOptNum;          //数值型配置项个数
    vector <SWConfInfo> binConfOptList;     //开关型配置项列表
    vector <SWConfInfo> numConfOptList;     //数值型配置项列表
public:
    ConfOpt();
    ~ConfOpt();
    int getBinConfOptNum();
    int getNumConfOptNum();
    vector <SWConfInfo> getBinConfOptList();
    vector <SWConfInfo> getNumConfOptList();
};

#endif