
@[TOC](STM32F767IGT6+ADC+官方DSP库 简易测试仪)

# 一、建立工程
参考：[link](https://blog.csdn.net/FormalLn/article/details/107171945)
## 1.导入DSP库
随意新建一个新的cube工程，进入Project Manager界面。
不要选择Generate under root选项，如图。
![Alt](https://imgconvert.csdnimg.cn/aHR0cHM6Ly90dHR0dHR0dHR0dHR0dC5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vY2xpb24vJUU5JTgwJTg5JUU5JUExJUI5LnBuZw?x-oss-process=image/format,png)
然后进入code generator，选择copy all used libraries into project folder
![Alt](https://imgconvert.csdnimg.cn/aHR0cHM6Ly90dHR0dHR0dHR0dHR0dC5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vY2xpb24vJUU5JTgwJTg5JUU2JThCJUE5JUU2JTg5JTgwJUU2JTlDJTg5JUU2JTk2JTg3JUU0JUJCJUI2LnBuZw?x-oss-process=image/format,png)
然后参考链接（暂时不想写了）
## 2.创建工程
按照普通的方法创建clion和stm32cube的工程，然后导入DSP库

编译器的面板配置文件选用stm32f7discovery，不要使用eval。

### 下载的时候一定要打开板子电源！！！
### 下载的时候一定要打开板子电源！！！
### 下载的时候一定要打开板子电源！！！
要不然下载失败可能就当选错配置文件了。


# 二、测量频率
## 1.说明
STM32并不能直接用一个引脚同时完成频率和峰峰值的测量。需要用ADC数据进行FFT（fast Fourier transform, 快速傅里叶变换），才能得到频率数据
傅里叶变换的一些资料：
1.深入浅出地讲解傅里叶变换 [link](https://www.cnblogs.com/h2zZhou/p/8405717.html)
2.陈爱军 《深入浅出通信原理》
3.fft背后的物理意义 [link](https://blog.csdn.net/iloveyoumj/article/details/53308142)
4.简明易懂fft[link](https://horizonwd.blog.csdn.net/article/details/81478582)
5.STM32F1X+ADC+FFT应用 [link](https://blog.csdn.net/qq_38495254/article/details/98883913)

## 2.采样频率
根据采样定理，采样频率必须是被采样信号最高频率的2倍。
假设采样频率为Fs，信号频率F，采样点数为N。那么FFT之后结果就是一个为N点的复数。每一个点就对应着一个频率点。这个点的模值，就是该频率值下的幅度特性。同时，FFT后的N个点，开始的那个点表示直流分量（即0Hz），而最后的那个点的再下一个点表示采样频率Fs，这中间被N-1个点平均分成N等份，每个点的频率依次增加。即，某点n所表示的频率为：Fn=(n-1)*Fs/N。这就表示，Fs/N就是频率分辨率。
详见原文链接：[link](https://blog.csdn.net/qq_38410730/article/details/90116695)
官方库是基4的fft，选用的fft点数必须是4的幂。

ADC无法配置到所需的采样频率（见1部分第五条链接），这里选用定时器触发ADC采样。
定时器选用TIM5，关于定时器挂在APB1还是APB2，可以参考《STM32F7中文参考手册》第143页（Adobe PDF显示的页码）。TIM5应该是在APB1.
难以确认时，可采用示波器测量其时钟频率，来判断在哪个线上。

## 3.TIM触发ADC的实现

