#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include "sample.h"
#include "nOWSample.h"
#include "OWSample.h"
#include "PWSample.h"
#include "PBSample.h"
#include "RDSample.h"
#include "config.h"

Sample::Sample()
{
    ConfOpt Opt;
    numConfOptNum = Opt.getNumConfOptNum();
    binConfOptNum = Opt.getBinConfOptNum();
}

Sample::~Sample()
{
}

/***************************
 * func: 使用OW和PB方法生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
***************************/
bool Sample::buildByOWAndPB(int measurementNum, int levelNum)
{
    //生成数值型配置项测试用例
    PBSample pb;
    if(!pb.build(measurementNum, levelNum))
    {
        cerr << "使用PB算法生成测试用例失败" << endl;
        return false;
    }
    
    //生成开关型配置项测试用例
    OWSample ow;
    ow.build();
    
    //打开测试用例文件
    ofstream out(SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << SAMPLE_PATH << "文件失败" << endl;
        return false;
    }
    else
    {
        ifstream in_PB(PB_SAMPLE_PATH);
        ifstream in_OW(OW_SAMPLE_PATH);
        //读取文件头
        string title;
        getline(in_PB, title);
        out << title << ',';
        getline(in_OW, title);
        out << title << endl;
        in_OW.close();
        //读取数据
        string dataPB;
        while(getline(in_PB, dataPB))
        {
            string dataOW;
            //跳过标题行
            in_OW.open(OW_SAMPLE_PATH);
            getline(in_OW, title);
            while(getline(in_OW, dataOW))
            {
                out << dataPB << ',';
                out << dataOW << endl;
            }
            
            in_OW.close();
        }
        
        in_PB.close();
    }
    
    out.close();
    
    return true;
}

/***************************
 * func: 使用OW和RD方法生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
***************************/
bool Sample::buildByOWAndRD(int measurementNum)
{
    //生成数值型配置项测试用例
    RDSample rd;
    if(!rd.build(measurementNum))
    {
        cerr << "使用RD算法生成测试用例失败" << endl;
        return false;
    }
    
    //生成开关型配置项测试用例
    OWSample ow;
    ow.build();
    
    //打开测试用例文件
    ofstream out(SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << SAMPLE_PATH << "文件失败" << endl;
        return false;
    }
    else
    {
        ifstream in_RD(RD_SAMPLE_PATH);
        ifstream in_OW(OW_SAMPLE_PATH);
        //读取文件头
        string title;
        getline(in_RD, title);
        out << title << ',';
        getline(in_OW, title);
        out << title << endl;
        in_OW.close();
        //读取数据
        string dataRD;
        while(getline(in_RD, dataRD))
        {
            string dataOW;
            //跳过标题行
            in_OW.open(OW_SAMPLE_PATH);
            getline(in_OW, title);
            while(getline(in_OW, dataOW))
            {
                out << dataRD << ',';
                out << dataOW << endl;
            }
            
            in_OW.close();
        }
        
        in_RD.close();
    }
    
    out.close();
    
    return true;
}

/***************************
 * func: 使用nOW和PB方法生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
***************************/
bool Sample::buildBynOWAndPB(int measurementNum, int levelNum)
{
    //生成数值型配置项测试用例
    PBSample pb;
    if(!pb.build(measurementNum, levelNum))
    {
        cerr << "使用PB算法生成测试用例失败" << endl;
        return false;
    }
    
    //生成开关型配置项测试用例
    nOWSample now;
    now.build();
    
    //打开测试用例文件
    ofstream out(SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << SAMPLE_PATH << "文件失败" << endl;
        return false;
    }
    else
    {
        ifstream in_PB(PB_SAMPLE_PATH);
        ifstream in_nOW(NOW_SAMPLE_PATH);
        //读取文件头
        string title;
        getline(in_PB, title);
        out << title << ',';
        getline(in_nOW, title);
        out << title << endl;
        in_nOW.close();
        //读取数据
        string dataPB;
        while(getline(in_PB, dataPB))
        {
            string datanOW;
            //跳过标题行
            in_nOW.open(NOW_SAMPLE_PATH);
            getline(in_nOW, title);
            while(getline(in_nOW, datanOW))
            {
                out << dataPB << ',';
                out << datanOW << endl;
            }
            
            in_nOW.close();
        }
        
        in_PB.close();
    }
    
    out.close();
    
    return true;
}

/***************************
 * func: 使用nOW和RD方法生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
***************************/
bool Sample::buildBynOWAndRD(int measurementNum)
{
    //生成数值型配置项测试用例
    RDSample rd;
    if(!rd.build(measurementNum))
    {
        cerr << "使用RD算法生成测试用例失败" << endl;
        return false;
    }
    
    //生成开关型配置项测试用例
    nOWSample now;
    now.build();
    
    //打开测试用例文件
    ofstream out(SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << SAMPLE_PATH << "文件失败" << endl;
        return false;
    }
    else
    {
        ifstream in_RD(RD_SAMPLE_PATH);
        ifstream in_nOW(NOW_SAMPLE_PATH);
        //读取文件头
        string title;
        getline(in_RD, title);
        out << title << ',';
        getline(in_nOW, title);
        out << title << endl;
        in_nOW.close();
        //读取数据
        string dataRD;
        while(getline(in_RD, dataRD))
        {
            string datanOW;
            //跳过标题行
            in_nOW.open(NOW_SAMPLE_PATH);
            getline(in_nOW, title);
            while(getline(in_nOW, datanOW))
            {
                out << dataRD << ',';
                out << datanOW << endl;
            }
            
            in_nOW.close();
        }
        
        in_RD.close();
    }
    
    out.close();
    
    return true;
}

/***************************
 * func: 使用PW和PB方法生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
***************************/
bool Sample::buildByPWAndPB(int measurementNum, int levelNum)
{
    //生成数值型配置项测试用例
    PBSample pb;
    if(!pb.build(measurementNum, levelNum))
    {
        cerr << "使用PB算法生成测试用例失败" << endl;
        return false;
    }
    
    //生成开关型配置项测试用例
    PWSample pw;
    pw.build();
    
    //打开测试用例文件
    ofstream out(SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << SAMPLE_PATH << "文件失败" << endl;
        return false;
    }
    else
    {
        ifstream in_PB(PB_SAMPLE_PATH);
        ifstream in_PW(PW_SAMPLE_PATH);
        //读取文件头
        string title;
        getline(in_PB, title);
        out << title << ',';
        getline(in_PW, title);
        out << title << endl;
        in_PW.close();
        //读取数据
        string dataPB;
        while(getline(in_PB, dataPB))
        {
            string dataPW;
            //跳过标题行
            in_PW.open(PW_SAMPLE_PATH);
            getline(in_PW, title);
            while(getline(in_PW, dataPW))
            {
                out << dataPB << ',';
                out << dataPW << endl;
            }
            
            in_PW.close();
        }
        
        in_PB.close();
    }
    
    out.close();
    
    return true;
}

/***************************
 * func: 使用nOW和RD方法生成测试用例集合
 * return: false = 生成测试用例失败    true = 生成成功
***************************/
bool Sample::buildByPWAndRD(int measurementNum)
{
    //生成数值型配置项测试用例
    RDSample rd;
    if(!rd.build(measurementNum))
    {
        cerr << "使用RD算法生成测试用例失败" << endl;
        return false;
    }
    
    //生成开关型配置项测试用例
    PWSample pw;
    pw.build();
    
    //打开测试用例文件
    ofstream out(SAMPLE_PATH);
    if(! out.is_open())
    {
        cerr << "打开" << SAMPLE_PATH << "文件失败" << endl;
        return false;
    }
    else
    {
        ifstream in_RD(RD_SAMPLE_PATH);
        ifstream in_PW(PW_SAMPLE_PATH);
        //读取文件头
        string title;
        getline(in_RD, title);
        out << title << ',';
        getline(in_PW, title);
        out << title << endl;
        in_PW.close();
        //读取数据
        string dataRD;
        while(getline(in_RD, dataRD))
        {
            string dataPW;
            //跳过标题行
            in_PW.open(PW_SAMPLE_PATH);
            getline(in_PW, title);
            while(getline(in_PW, dataPW))
            {
                out << dataRD << ',';
                out << dataPW << endl;
            }
            
            in_PW.close();
        }
        
        in_RD.close();
    }
    
    out.close();
    
    return true;
}