/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : PWSample.h
* Descripe     : use Pair-Wise Sampling(PW)
******************************************************/

#ifndef __PWSAMPLE_H__
#define __PWSAMPLE_H__

#include <vector>
#include "SWConfOpt.h"

using namespace std;

class PWSample
{
private:
    int binConfOptNum;
    vector <SWConfInfo> binConfOpt;
public:
    PWSample();
    ~PWSample();
    bool build();
};

#endif