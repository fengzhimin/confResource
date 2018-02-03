#include <iostream>
#include <fstream>
#include "nOWSample.h"
#include "config.h"

nOWSample::nOWSample()
{
    ConfOpt Opt;
    binConfOptNum = Opt.getBinConfOptNum();
    binConfOpt = Opt.getBinConfOptList();
}

nOWSample::~nOWSample()
{
}

/***************************
 * func: 生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
***************************/
bool nOWSample::build()
{
    //打开软件配置项文件
    ofstream out(NOW_SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << NOW_SAMPLE_PATH << "文件失败" << endl;
        return false;
    }
    else
    {
        int temp_size = binConfOptNum - 1;
        for(int i = 0; i < temp_size; i++)
            out << binConfOpt[i].confName << ",";
        out << binConfOpt[temp_size].confName << endl;
        
        for(int i = 0; i < binConfOptNum; i++)
        {
            for(int j = 0; j < temp_size; j++)
            {
                if(i == j)
                    out << "N" << ',';
                else
                    out << "Y" << ',';
            }
            
            if(i == temp_size)
                out << "N";
            else
                out << "Y";
            
            out << endl;
        }
        
        out.close();
    }
    
    return true;
}