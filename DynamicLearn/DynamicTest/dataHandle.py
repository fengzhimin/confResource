#!/usr/bin/python
# -*- coding: UTF-8 -*-

import re
import sys
from sys import argv

#测试结果文件
inputFile1 = "PB_2500.csv"
#配置项组合文件
inputFile2 = "ConfInteractResult.csv"
#存放平均值文件
outputFile = "dataHandle.csv"
#相同配置项组合个数
confCount = 5

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
    measurementFD = open(inputFile1, "r")
    #读取所有数据
    measurementLines = measurementFD.readlines()
    measurementFD.close()
    #打开配置项组合文件
    interactFD = open(inputFile2, "r")
    #读取所有数据
    interactLines = interactFD.readlines()
    interactFD.close()
    #获取组合配置项个数
    interactConfNum = len(interactLines[0].split(","))
    #将配置项标题写入结果文件中
    resultFD = open(outputFile, "w")
    interactConfIndex = []
    if interactConfNum > 0:
        measurements = measurementLines[0].split(",")
        lenMeasurement = len(measurements)
        for label in measurements[:lenMeasurement-4]:
            resultFD.write(label + ",")
        line = interactLines[0].replace("\n", ",")
        resultFD.write(line)
        for label in measurements[lenMeasurement-4:lenMeasurement-1]:
            resultFD.write(label + ",")
        resultFD.write(str(measurements[lenMeasurement-1]))
        interactLines[0] = interactLines[0].replace("\n", "")
        interactConfs = interactLines[0].split(",")
        for interactConf in interactConfs:
            confs = interactConf.split(":")
            indexs = []
            for conf in confs:
                indexs.append(measurements.index(conf))
            interactConfIndex.append(indexs)
    else:
        resultFD.write(measurementLines[0])
    resultFD.close()
    for line in measurementLines[1:]:
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
            
            counter = 0
            while counter < interactConfNum:
                interactSum = 1
                indexs = interactConfIndex[counter]
                for index in indexs:
                    interactSum *= int(ConfValue[index])
                file.write(str(interactSum)+",")
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
