/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : nOWSample.h
* Descripe     : use negative Option-Wise Sampling(nOW)
******************************************************/

#ifndef __NOWSAMPLE_H__
#define __NOWSAMPLE_H__

#include <string>
#include <vector>
#include "SWConfOpt.h"

using namespace std;

class nOWSample
{
private:
    int binConfOptNum;
    vector <SWConfInfo> binConfOpt;
public:
    nOWSample();
    ~nOWSample();
    bool build();
};

#endif