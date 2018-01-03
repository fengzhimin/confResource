#!/usr/bin/python
# -*- coding: UTF-8 -*-

import re
import sys
import os
import threading
import time

#软件配置文件所在目录
softwareConfPath = "/etc/mysql/mysql.conf.d/"
#软件默认配置文件路径
srcConfFilePath = "/home/fzm/Desktop/mysqld.cnf"
#新配置文件名称
desConfFilePath = "mysql.cnf"
#重启软件命令
restartSoftWareCommand = "sudo service mysql restart"
#压力测试命令
DynamicTestCommand = "testMysql"
#测试软件的名称
SoftWareName = "mysqld"
#监控结果存放的文件名
MonitorResultFile = "resource.txt"
#最终动态测试结果值存放的文件名
resultFile = "result.txt"

#总的内存使用和
sumMem = 0
#监控次数
count = 0
#平均内存使用
averageMem = 0

#需要修改的配置项名称
confName = [ "key_buffer_size", "sort_buffer_size", "read_buffer_size", "tmp_table_size", "join_buffer_size" ]
#需要修改的值
valueList = [ '16K', '32K', '64K', '128K', '256K', '512K', '1M', '8M', '16M', '32M',
              '64M', '128M', '256M', '512M' ]

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
                if name[0] == confName:
                    oldValue = name[1].strip()
                    line = line.replace(oldValue, value)
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
    #生成监控信息文件result.txt
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

"""
saveResult: 获取资源使用情况
"""
def saveResult(confValue, resourceValue):
    file = open(resultFile, "a")
    file.write(confValue + ":" + str(resourceValue) + "\n")
    file.close()

if __name__ == '__main__':
    if os.path.exists(resultFile):
        os.remove(resultFile)
    for configName in confName:
        file = open(resultFile, "a")
        file.write("\n[" + configName + "]\n")
        file.close()
        for value in valueList:
            #生成新的配置文件
            buildConfFile(srcConfFilePath, desConfFilePath, configName, value)
            #拷贝新配置文件
            os.system("sudo cp "+ desConfFilePath + " " + softwareConfPath)
            #重新启动程序
            os.system(restartSoftWareCommand)
            #创建一个新的线程去运行动态测试程序
            thread1 = threading.Thread(target=runDynamicTest, args=())
            thread1.start()
            sumMem = 0
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
            averageMem = sumMem/count
            saveResult(value, averageMem)
