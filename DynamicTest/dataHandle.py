#!/usr/bin/python
# -*- coding: UTF-8 -*-

import re
import sys
from sys import argv

#测试结果文件
resultFile = "result.csv"
#存放平均值文件
averageFile = "resultData.csv"
#相同配置项组合个数
confCount = 5

memSum = 0
CPUSum = 0
count = 0

'''
function: 同一组配置项组合被测试多次，取平均值
'''
if __name__ == '__main__':
    #打开待测试用例集合
    measurementFD = open(resultFile, "r")
    #读取所有数据
    lines = measurementFD.readlines()
    measurementFD.close()
    #将配置项标题写入结果文件中
    resultFD = open(averageFile, "w")
    resultFD.write(lines[0])
    resultFD.close()
    for line in lines[1:]:
        ConfValue = line.split(",")
        if count == (confCount-1):
            CPUSum += float(ConfValue[3])
            memSum += float(ConfValue[4])
            file = open(averageFile, "a")
            mem = str(memSum/confCount)
            cpu = str(CPUSum/confCount)
            file.write(ConfValue[0]+",")
            file.write(ConfValue[1]+",")
            file.write(ConfValue[2]+",")
            file.write(str(cpu)+",")
            file.write(str(mem)+"\n")
            file.close()
            count = 0
            memSum = 0
            CPUSum = 0
        else:
            count += 1
            CPUSum += float(ConfValue[3])
            memSum += float(ConfValue[4])