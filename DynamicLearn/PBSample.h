/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : PBSample.h
* Descripe     : use Plackett-Burman Sampling
******************************************************/

#ifndef __PBSAMPLE_H__
#define __PBSAMPLE_H__

#include <string>
#include <vector>
#include "SWConfOpt.h"

using namespace std;

class PBSample
{
private:
    int numConfOptNum;
    vector <SWConfInfo> numConfOpt;
public:
    PBSample();
    ~PBSample();
    bool build(int measurementNum = 49, int levelNum = 7);
};

#endif