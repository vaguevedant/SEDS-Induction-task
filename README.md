# Vedant Chaudhari_2025B5PS1510H_AVIONICS

TASK-1

I used pandas library to extract data. I also used pd.to_numeric(), to take care of the points where sensor produced corrupted data(eg. #VALUE!). Then I plotted the data and saw sharp jumps at two points. To resolve this I initially thought about using the concept of standard deviation to reject such points. But on implementing that I saw that a lot of points were being rejected, so there were large gaps in the graph. Finally I simply went with comparing the current depth with the previous one and if that is within an acceptable range(which we can set), the value will be accepted.


To reduce the noise, I initially went with taking the average of the previous 5 readings and taking it as the current reading. But in my recent college lectures, I learned the concept of convolution, which can be used to take a running average of a function. So I implemented that in my code. It beautifully averages the incoming noisy data.

My code technically can be used for graphing real time data. But as the data accumulates more and more, I might get slow since it always starts from the first data point everytime a new datapoint is added. IDK how to optimize it.
   

