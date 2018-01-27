#!/usr/bin/python
# -*- coding: UTF-8 -*-

import pandas as pd
import numpy as np
from sklearn.cross_validation import train_test_split
from sklearn.linear_model import LinearRegression  

data = pd.read_csv('resultData.csv')
feature_cols = ['sort_buffer_size', 'key_buffer_size', 'read_buffer_size']
X = data[feature_cols]
y = data['MEM']

X_train,X_test, y_train, y_test = train_test_split(X, y, test_size=0.5, random_state=0)  
print X_train.shape  
print y_train.shape  
print X_test.shape  
print y_test.shape  

linreg = LinearRegression()  
model=linreg.fit(X_train, y_train)  
print model  
print linreg.intercept_  
print linreg.coef_  

y_pred = linreg.predict(X_test)  
print y_pred  

print type(y_pred),type(y_test)  
print len(y_pred),len(y_test)  
print y_pred.shape,y_test.shape  
from sklearn import metrics  
import numpy as np  
sum_mean=0  
for i in range(len(y_pred)):  
    sum_mean+=(y_pred[i]-y_test.values[i])**2 
sum_erro=np.sqrt(sum_mean/len(y_test))
# calculate RMSE by hand  
print "RMSE by hand:",sum_erro

import matplotlib.pyplot as plt  
plt.figure()  
plt.plot(range(len(y_pred)),y_pred,'b',label="predict")  
plt.plot(range(len(y_pred)),y_test,'r',label="test")  
plt.legend(loc="upper right") #显示图中的标签  
plt.xlabel("the number of memory")  
plt.ylabel('value of memory')
plt.show()  
