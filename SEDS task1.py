import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import pandas as pd
import numpy as np
 
plt.style.use('fivethirtyeight')
 
fig, ax = plt.subplots()
 
threshold = 50   #If needed, we can easily change the acceptable range of error
 
data = pd.read_csv("Depth Data1.csv")
 
 
def animate(i):                                                     
 
    data = pd.read_csv("Depth Data.csv")
 
    x = data['Point'][:i+1]
    y = pd.to_numeric(data['Depth (m)'][:i+1], errors='coerce')   #this replaces values like #VALUE! to nan. Only accepts float values
 
    y_fixed = []
    last_good = None
 
    for value in y:
 
        if last_good is None:       #just a failsafe if the first only datapoint is corrupted. It will keep going until it receives a valid first point.
            if np.isnan(value):
                y_fixed.append(np.nan)
                continue
            else:
                y_fixed.append(value)     #all the valid datapoints get added to y_fixed array
                last_good = value
                continue
 
        if np.isnan(value) or abs(value - last_good) > threshold:
            y_fixed.append(last_good)                                      #if the current value is corropted or erractic, it is replaced with prv. valid point so as to not break the graph
        else:
            y_fixed.append(value)
            last_good = value
 
    y_padded = np.pad(y_fixed, (4, 0), mode='edge')               #padding baiscally means adding the first element of the array len(array)-1 times. So that the noisefree data graph and raw data graph start from the same point
    kernel = [0.2, 0.2, 0.2, 0.2, 0.2]
    y_noisefree = np.convolve(y_padded, kernel, mode='valid')[:i+1]      #convoluting to find the running average of the data. to give a smoooth graph
 
    ax.clear()
 
    ax.plot(x, y, marker='o', label='Raw data')
    ax.plot(x, y_noisefree, label='Data after cleaning')

    ax.legend()
 
    ax.set_title("DEPTH VS TIME LIVE GRAPH ")
    ax.set_xlabel("Point")
    ax.set_ylabel("Depth (m)")
 
ani = FuncAnimation(fig, animate, interval=10)
 
 
plt.show()
