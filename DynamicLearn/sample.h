/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : sample.h
* Descripe     : using sample build measurements
******************************************************/

#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include "PBSample.h"
#include "RDSample.h"
#include "OWSample.h"
#include "nOWSample.h"
#include "PWSample.h"

class Sample
{
private:
    int numConfOptNum;
    int binConfOptNum;
public:
    Sample();
    ~Sample();
    bool buildByOWAndPB(int measurementNum = 49, int levelNum = 7);
    bool buildByOWAndRD(int measurementNum = 50);
    bool buildBynOWAndPB(int measurementNum = 49, int levelNum = 7);
    bool buildBynOWAndRD(int measurementNum = 50);
    bool buildByPWAndPB(int measurementNum = 49, int levelNum = 7);
    bool buildByPWAndRD(int measurementNum = 50);
};


#endif