/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : PDSample.h
* Descripe     : use Random Design Sampling
******************************************************/

#ifndef __RDSAMPLE_H__
#define __RDSAMPLE_H__

#include <string>
#include <vector>
#include "SWConfOpt.h"

using namespace std;

class RDSample
{
private:
    int numConfOptNum;
    vector <SWConfInfo> numConfOpt;
    int step_size;     //取值梯度
public:
    RDSample(int _step_size = 16);
    ~RDSample();
    bool build(int measurementNum = 50);
};

#endif