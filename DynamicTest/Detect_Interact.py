#!/usr/bin/python
# -*- coding: UTF-8 -*-

import re
import sys
import os
import threading
import time

#软件配置文件所在目录
softwareConfPath = "/etc/mysql/"
#软件默认配置文件路径
srcConfFilePath = "./my.cnf.back"
#新配置文件名称
desConfFilePath = "my.cnf"
#重启软件命令
restartSoftWareCommand = "sudo service mysql restart > /dev/null 2>&1"
#压力测试命令
DynamicTestCommand = "./exe > /dev/null 2>&1"
#测试软件的名称
SoftWareName = "mysqld"
#监控结果存放的文件名
MonitorResultFile = "resource.txt"
#存放最终那些配置项是相互作用影响而不是单独影响资源
resultFile = "ConfInteractResult.csv"
#存放待测试配置项文件
confInfo = "confInfo.csv"
#为了获取平均值，将一组配置项执行多次
measureCounter = 5

#总的CPU使用率和
sumCPU = 0
#总的内存使用和
sumMem = 0
#总的IO数
sumIO = 0
#总的net数
sumNET = 0
#监控次数
count = 0
#平均CPU使用率
averageCPU = 0
#平均内存使用
averageMem = 0

'''
buildConfFile: 用于生成配置项修改后新配置文件
srcConfFilePath: 原始配置文件路径
desConfFilePath: 修改配置项后新的配置文件路径
confName: 需要修改的配置项名
value: 需要修改的值
'''
def buildConfFile(srcConfFilePath, desConfFilePath, confName, value):
    srcFile = open(srcConfFilePath, "r")
    desFile = open(desConfFilePath, "w")
    for line in srcFile:
        temp_line = line.lstrip()
        #如果是注释则直接复制
        if len(temp_line) != 0:
            if temp_line[0] == "#":
                desFile.write(line)
            else:
                name = line.split('=', 1)
                name[0] = name[0].strip()
                #判断配置项是否为要修改的配置项
                if name[0] in confName:
                    oldValue = re.sub("\D", "", name[1].strip())
                    line = line.replace(oldValue, value[confName.index(name[0])])
                desFile.write(line)
        else:
            desFile.write(line)

    srcFile.close()
    desFile.close()

"""
runDynamic: 运行动态测试程序
"""
def runDynamicTest():
    os.system(DynamicTestCommand)

"""
getMonitorResult: 获取资源使用情况
"""
def getMonitorResult():
    #生成监控信息文件result.csv
    os.system("top -p `pgrep " + SoftWareName + " | tr \"\\n\" \",\" | sed 's/,$//'` -b -n 1 > " + MonitorResultFile)
    monitorFile = open(MonitorResultFile, "r")
    lines = monitorFile.readlines()
    monitorFile.close()
    #从第7行开始读取数据
    lines = lines[7:]
    for line in lines:
        line = line.split()
        global sumMem
        sumMem += long(line[5])
        global sumCPU
        sumCPU += float(line[8])
    #获取IO数据
    command = os.popen("pgrep " + SoftWareName + " | tr \"\\n\" \",\" | sed 's/,$//'") #执行该命令
    commandResult = command.readlines()
    pids = commandResult[0].split(",")
    for pid in pids:
        command = os.popen("sudo iotop -k -p " + pid + " -b -n 1 | tail -n 1 | tr -s ' ' | sed -r 's/^ | $//' | cut -d ' ' -f4,6")
        commandResult = command.readlines()
        ioData = commandResult[0].split()
        global sumIO
        sumIO += float(ioData[0])
        sumIO += float(ioData[1])

'''
计算出测试用例执行所消耗的资源
'''
def getResource():
    sumAverageCPU = 0
    sumAverageMem = 0
    sumAverageIO = 0
    sumAverageNET = 0
    counter = 0
    while counter < measureCounter:
        #重新启动程序
        os.system(restartSoftWareCommand)
        print("restart success")
        #创建一个新的线程去运行动态测试程序
        thread1 = threading.Thread(target=runDynamicTest, args=())
        thread1.start()
        global sumMem, sumCPU, sumIO, sumNET
        sumMem = 0
        sumCPU = 0
        sumIO = 0
        sumNET = 0
        count = 0
        """
        每隔30秒去检测程序占用资源情况
        直到动态测试程序结束为止
        """
        while thread1.is_alive():
            getMonitorResult()
            count += 1
            time.sleep(1)
        #计算出资源平均使用大小
        sumAverageCPU += sumCPU/count
        sumAverageMem += sumMem/count
        sumAverageIO += sumIO/count
        sumAverageNET += sumNET/count
        counter += 1
    return sumAverageCPU/measureCounter,sumAverageMem/measureCounter,sumAverageIO/measureCounter,sumAverageNET/measureCounter

"""
saveResult: 获取资源使用情况
CPUValue: CPU使用率
MEMValue: 内存使用率
"""
def saveResult(CPUValue, MEMValue, IOValue, NETValue):
    file = open(resultFile, "a")
    file.write("," + str(CPUValue) + "," + str(MEMValue) + "," + str(IOValue) + "," + str(NETValue) + "\n")
    file.close()

if __name__ == '__main__':
    if os.path.exists(resultFile):
        os.remove(resultFile)
    #打开待测试配置项集合
    confInfoFD = open(confInfo, "r")
    #读取所有数据
    lines = confInfoFD.readlines()
    confInfoFD.close()
    #将配置项标题写入结果文件中
    resultFD = open(resultFile, "a")
    resultFD.write(lines[0])
    resultFD.close()
    #得到待测试的配置项名称
    confName = lines[0].rstrip("\n")
    confName = confName.split(",")
    #读取每个配置项的取值范围
    #confRanges[0][0]表示０号配置项的最小值
    #confRanges[0][1]表示０号配置项的最大值
    confRanges = lines[1].rstrip("\n")
    confRanges = confRanges.split(",")
    confLen = len(confRanges)
    confMaxValue = []
    confMinValue = []
    ConfNum = 0
    while ConfNum < confLen:
        confRanges[ConfNum] = confRanges[ConfNum].split("-")
        confMaxValue.append(confRanges[ConfNum][1])
        confMinValue.append(confRanges[ConfNum][0])
        ConfNum += 1

    '''
    生成新的配置文件
    deltasMaxA = AXBXC - BXC
    计算AXBXC影响的资源
    '''
    buildConfFile(srcConfFilePath, desConfFilePath, confName, confMaxValue)
    #拷贝新配置文件
    os.system("sudo cp "+ desConfFilePath + " " + softwareConfPath)
    maxCPU, maxMEM, maxIO, maxNET = getResource()

    '''
    生成新的配置文件
    计算每个配置项是最小值(关闭)状态下的资源使用情况
    '''
    buildConfFile(srcConfFilePath, desConfFilePath, confName, confMinValue)
    #拷贝新配置文件
    os.system("sudo cp "+ desConfFilePath + " " + softwareConfPath)
    minCPU, minMEM, minIO, minNET = getResource()

    #记录处理到第几个配置项
    confIndex = 0
    for confRange in confRanges:
        '''
        生成新的配置文件
        deltasMinA = A
        计算A影响的资源
        '''
        confValue = list(confMinValue)
        confValue[confIndex] = confRange[1]
        buildConfFile(srcConfFilePath, desConfFilePath, confName, confValue)
        #拷贝新配置文件
        os.system("sudo cp "+ desConfFilePath + " " + softwareConfPath)
        averageCPU, averageMem, averageIO, averageNET = getResource()
        #获取单独(最小)影响资源的值
        deltasMinCPU = averageCPU - minCPU
        deltasMinMem = averageMem - minMEM
        deltasMinIO = averageIO - minIO
        deltasMinNET = averageNET - minNET

        '''
        生成新的配置文件
        deltasMaxA = AXBXC - BXC
        计算BXC影响的资源
        '''
        confValue = list(confMaxValue)
        confValue[confIndex] = confRange[0]
        buildConfFile(srcConfFilePath, desConfFilePath, confName, confValue)
        #拷贝新配置文件
        os.system("sudo cp "+ desConfFilePath + " " + softwareConfPath)
        averageCPU, averageMem, averageIO, averageNET = getResource()
        #获取最大影响资源的值
        deltasMaxCPU = maxCPU - averageCPU
        deltasMaxMem = maxMEM - averageMem
        deltasMaxIO = maxIO - averageIO
        deltasMaxNET = maxNET - averageNET

        print("配置项:" + confName[confIndex] + " deltasMaxMem＝" + str(deltasMaxMem) + " deltasMinMem=" + str(deltasMinMem))
        file = open(resultFile, "a")
        if abs(deltasMaxMem - deltasMinMem) < 25000:
            #单独影响资源
            file.write("N,")
        else:
            file.write("Y,")
        file.close()
        confIndex += 1

    file = open(resultFile, "r")
    lines = file.readlines()
    file.close()
    file = open(resultFile, "w")
    file.truncate()
    file.write(lines[0])
    line = lines[1].rstrip(",") + "\n"
    file.write(line)
    file.close()
