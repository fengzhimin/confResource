#include <iostream>
#include <fstream>
#include "PWSample.h"
#include "config.h"

PWSample::PWSample()
{
    ConfOpt Opt;
    binConfOptNum = Opt.getBinConfOptNum();
    binConfOpt = Opt.getBinConfOptList();
}

PWSample::~PWSample()
{
}

/***************************
 * func: 生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
***************************/
bool PWSample::build()
{
    //打开软件配置项文件
    ofstream out(PW_SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << PW_SAMPLE_PATH << "文件失败" << endl;
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
            for(int j = i + 1; j < binConfOptNum; j++)
            {
                for(int n = 0; n < temp_size; n++)
                {
                    if(n == i || n == j)
                        out << "Y" << ',';
                    else
                        out << "N" << ',';
                }
                if(i == temp_size || j == temp_size)
                    out << "Y";
                else
                    out << "N";
                
                out << endl;
            }
        }
        
        out.close();
    }
    
    return true;
}