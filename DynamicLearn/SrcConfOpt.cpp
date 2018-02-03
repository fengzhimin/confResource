#include <iostream>
#include <fstream>
#include "SWConfOpt.h"

/****************************
 * func: 构造函数
****************************/
ConfOpt::ConfOpt()
{
    //打开软件配置项文件
    ifstream in(SRC_CONF_OPT_PATH);
    if(! in.is_open())
        cerr << "打开软件配置项文件失败" << endl;
    else
    {
        while(! in.eof())
        {
            SWConfInfo confInfo;
            in >> confInfo.confName >> confInfo.confType;
            /*
             * 0 表示开关型配置项
             * 1 表示数值型配置项
             */
            switch(confInfo.confType)
            {
                case 0:
                    binConfOptList.push_back(confInfo);
                    break;
                case 1:
                    in >> confInfo.minValue >> confInfo.maxValue;
                    numConfOptList.push_back(confInfo);
                    break;
                default :
                    break;
            }
                
        }
    }
    
    binConfOptNum = binConfOptList.size();
    numConfOptNum = numConfOptList.size();
    
    in.close();
}

ConfOpt::~ConfOpt()
{
}

/******************************
 * func: 获取开关型配置项的个数
******************************/
int ConfOpt::getBinConfOptNum()
{
    return binConfOptNum;
}

/******************************
 * func: 获取数值型配置项的个数
******************************/
int ConfOpt::getNumConfOptNum()
{
    return numConfOptNum;
}

/******************************
 * func: 获取开关型配置项列表
*******************************/
vector <SWConfInfo> ConfOpt::getBinConfOptList()
{
    return binConfOptList;
}

/******************************
 * func: 获取数值型配置项列表
*******************************/
vector <SWConfInfo> ConfOpt::getNumConfOptList()
{
    return numConfOptList;
}