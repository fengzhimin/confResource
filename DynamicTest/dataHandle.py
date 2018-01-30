#!/usr/bin/python
# -*- coding: UTF-8 -*-

import re
import sys
from sys import argv

#测试结果文件
inputFile = "result.csv"
#存放平均值文件
outputFile = "dataHandle.csv"
#相同配置项组合个数
confCount = 10

memSum = 0
CPUSum = 0
IOSum = 0
NETSum = 0
count = 0

'''
function: 同一组配置项组合被测试多次，取平均值
'''
if __name__ == '__main__':
    #打开待测试用例集合
    measurementFD = open(inputFile, "r")
    #读取所有数据
    lines = measurementFD.readlines()
    measurementFD.close()
    #将配置项标题写入结果文件中
    resultFD = open(outputFile, "w")
    resultFD.write(lines[0])
    resultFD.close()
    for line in lines[1:]:
        ConfValue = line.split(",")
        confLen = len(ConfValue)
        if count == (confCount-1):
            CPUSum += float(ConfValue[confLen-4])
            memSum += float(ConfValue[confLen-3])
            IOSum += float(ConfValue[confLen-2])
            NETSum += float(ConfValue[confLen-1])
            file = open(outputFile, "a")
            mem = str(memSum/confCount)
            cpu = str(CPUSum/confCount)
            io = str(IOSum/count)
            net = str(NETSum/count)
            counter = 0
            while counter <= (confLen-5):
                file.write(ConfValue[counter]+",")
                counter += 1

            file.write(str(cpu)+",")
            file.write(str(mem)+",")
            file.write(str(io)+",")
            file.write(str(net)+"\n")
            file.close()
            count = 0
            memSum = 0
            CPUSum = 0
            IOSum = 0
            NETSum = 0
        else:
            count += 1
            CPUSum += float(ConfValue[confLen-4])
            memSum += float(ConfValue[confLen-3])
            IOSum += float(ConfValue[confLen-2])
            NETSum += float(ConfValue[confLen-1])
