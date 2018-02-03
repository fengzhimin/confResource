#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include "PBSample.h"
#include "config.h"

PBSample::PBSample()
{
    ConfOpt Opt;
    numConfOptNum = Opt.getNumConfOptNum();
    numConfOpt = Opt.getNumConfOptList();
}

PBSample::~PBSample()
{
}

/***************************
 * func: 生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
 * @para measurementNum: 生成的测试案例个数
 * @para levelNum: 取值的梯度范围
***************************/
bool PBSample::build(int measurementNum, int levelNum)
{
    //measurementNum能被levelNum整除
    if((measurementNum % levelNum) != 0)
    {
        cerr << "测试用例个数和取值等级不符合要求" << endl;
        return false;
    }
    //打开软件配置项文件
    ofstream out(PB_SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << PB_SAMPLE_PATH << "文件失败" << endl;
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
        
        //seedNum为影响因子个数
        int seedNum = measurementNum - 1;
        int *seed = new int[seedNum];
        //最小值个数
        int min_level = seedNum/levelNum;
        int max_level = min_level + 1;
        vector <int> seedRange;
        for(int i = 0; i < min_level; i++)
        {
            seedRange.push_back(0);
        }
        for(int i = 0; i < max_level; i++)
        {
            for(int j = 1; j < levelNum; j++)
                seedRange.push_back(j);
        }
        
        //设置第一行seed值
        for(int i = 0; i < seedNum; i++)
        {
            int index = rand()%seedRange.size();
            seed[i] = seedRange[index];
            vector <int>::iterator it = seedRange.begin() + index;
            seedRange.erase(it);
        }
        
        int **temp_seed = new int*[seedNum];
        for(int i = 0; i < seedNum; i++)
            temp_seed[i] = new int[numConfOptNum];
            
        for(int i = 0; i < seedNum; i++)
        {
            int index = i;
            //只记录有效的配置项取值
            //新的行是由上一行右移一位得到
            for(int j = 0; j < numConfOptNum; j++)
            {
                temp_seed[i][j] = seed[index%seedNum];
                index++;
            }
        }
        
        //写入文件
        for(int i = 0; i < seedNum; i++)
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
                    out << temp_seed[i][j]*(numConfOpt[j].maxValue - numConfOpt[j].minValue)/levelNum  + numConfOpt[j].minValue << ',';
                }
                out << temp_seed[i][temp_size]*(numConfOpt[temp_size].maxValue - numConfOpt[temp_size].minValue)/levelNum  + numConfOpt[temp_size].minValue<< endl;
            }
        }
        //生成最后一组参考值
        for(int i = 0; i < temp_size; i++)
        {
            out << numConfOpt[i].minValue << ',';
        }
        out << numConfOpt[temp_size].minValue << endl;
        
        delete seed;
        out.close();
    }
    
    return true;
}