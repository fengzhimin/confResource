/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : OWSample.h
* Descripe     : use Option-Wise Sampling(OW)
******************************************************/

#ifndef __OWSAMPLE_H__
#define __OWSAMPLE_H__

#include <string>
#include <vector>
#include "SWConfOpt.h"

using namespace std;

class OWSample
{
private:
    int binConfOptNum;
    vector <SWConfInfo> binConfOpt;
public:
    OWSample();
    ~OWSample();
    bool build();
};

#endif