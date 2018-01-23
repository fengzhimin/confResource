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
restartSoftWareCommand = "sudo service mysql restart"
#压力测试命令
DynamicTestCommand = "./exe"
#测试软件的名称
SoftWareName = "mysqld"
#监控结果存放的文件名
MonitorResultFile = "resource.txt"
#最终动态测试结果值存放的文件名
resultFile = "result.csv"
#配置项组合存放的文件名
measurement = "SAMPLE.csv"

#总的CPU使用率和
sumCPU = 0
#总的内存使用和
sumMem = 0
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

"""
saveResult: 获取资源使用情况
CPUValue: CPU使用率
MEMValue: 内存使用率
"""
def saveResult(CPUValue, MEMValue):
    file = open(resultFile, "a")
    file.write("," + str(CPUValue) + "," + str(MEMValue) + "\n")
    file.close()

if __name__ == '__main__':
    if os.path.exists(resultFile):
        os.remove(resultFile)
    #打开待测试用例集合
    measurementFD = open(measurement, "r")
    #读取所有数据
    lines = measurementFD.readlines()
    measurementFD.close()
    #将配置项标题写入结果文件中
    resultFD = open(resultFile, "a")
    resultFD.write(lines[0].replace("\n", ", CPU, MEM\n"))
    resultFD.close()
    #得到待测试的配置项名称
    confName = lines[0].rstrip("\n")
    confName = confName.split(",")
    for line in lines[1:]:
        line = line.rstrip("\n")
        file = open(resultFile, "a")
        file.write(line)
        file.close()
        ConfValue = line.split(",")
        #生成新的配置文件
        buildConfFile(srcConfFilePath, desConfFilePath, confName, ConfValue)
        #拷贝新配置文件
        os.system("sudo cp "+ desConfFilePath + " " + softwareConfPath)
        #重新启动程序
        os.system(restartSoftWareCommand)
        print("restart success")
        #创建一个新的线程去运行动态测试程序
        thread1 = threading.Thread(target=runDynamicTest, args=())
        thread1.start()
        sumMem = 0
        sumCPU = 0
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
        averageCPU = sumCPU/count
        averageMem = sumMem/count
        saveResult(averageCPU, averageMem)