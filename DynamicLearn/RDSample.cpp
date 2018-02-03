#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include "RDSample.h"
#include "config.h"

/*****************************
 * func: 构造函数
 * @para _step_size: 取值梯度
*****************************/
RDSample::RDSample(int _step_size)
{
    ConfOpt Opt;
    numConfOptNum = Opt.getNumConfOptNum();
    numConfOpt = Opt.getNumConfOptList();
    step_size = _step_size;
}

RDSample::~RDSample()
{
}

/***************************
 * func: 生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
 * @para measurementNum: 生成的测试用例个数
***************************/
bool RDSample::build(int measurementNum)
{
    //打开测试用例文件
    ofstream out(RD_SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << RD_SAMPLE_PATH << "文件失败" << endl;
        return false;
    }
    else
    {
        int temp_size = numConfOptNum - 1;
        for(int i = 0; i < temp_size; i++)
            out << numConfOpt[i].confName << ",";
        out << numConfOpt[temp_size].confName << endl;
        //初始化随机数
        srand((unsigned)time(0));
        
        int **temp_seed = new int*[measurementNum];
        for(int i = 0; i < measurementNum; i++)
            temp_seed[i] = new int[numConfOptNum];
            
        for(int i = 0; i < measurementNum; i++)
        {
            for(int j = 0; j < numConfOptNum; j++)
            {
                //在有效的取值范围区间内最多能到生成多少个大小为step_size间隔区间
                int rand_maxRange = (numConfOpt[j].maxValue-numConfOpt[j].minValue)/step_size;
                //生成有效区间内的随机值
                int rand_num = (rand()%rand_maxRange)*step_size + numConfOpt[j].minValue;
                temp_seed[i][j] = rand_num;
            }
        }
        
        //写入文件
        for(int i = 0; i < measurementNum; i++)
        {
            //查重是否已经存在
            bool isExist = false;
            for(int n = 0; n < i; n++)
            {
                int j = 0;
                for( ; j < numConfOptNum; j++)
                {
                    if(temp_seed[n][j] != temp_seed[i][j])
                    {
                        break;
                    }
                }
                
                if(j == numConfOptNum)
                {
                    isExist = true;
                    break;
                }
            }
            
            if(!isExist)
            {
                for(int j = 0; j < temp_size; j++)
                {
                    //生成有效区间内的随机值
                    out << temp_seed[i][j] << ',';
                }
                //生成有效区间内的随机值
                out << temp_seed[i][temp_size] << endl;
            }
        }
        
        for(int i = 0; i < measurementNum; i++)  
        {  
            delete temp_seed[i];  
            temp_seed[i] = NULL;  
        }  
        delete []temp_seed;  
    
        out.close();
    }
    
    return true;
}