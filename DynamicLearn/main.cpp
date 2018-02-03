#include <cstdlib>
#include <string>
#include <cstdio>
#include <iostream>
#include <sample.h>

using namespace std;

int main(int argc, char *argv[])
{
    //生成测试用例集合
    Sample sm;
    sm.buildBynOWAndRD(1000);
    
    if(argc < 2)
        cerr << "请输入测试脚本命令" << endl;
    
    //执行测试脚本
    system(argv[1]);
    
    return 0;
}
