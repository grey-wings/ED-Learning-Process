
@[TOC](简易测试仪)

# 建立工程
参考：https://blog.csdn.net/FormalLn/article/details/107171945
## 1.导入DSP库
随意新建一个新的cube工程，进入Project Manager界面。
不要选择Generate under root选项，如图。
![Alt](https://imgconvert.csdnimg.cn/aHR0cHM6Ly90dHR0dHR0dHR0dHR0dC5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vY2xpb24vJUU5JTgwJTg5JUU5JUExJUI5LnBuZw?x-oss-process=image/format,png)
然后进入code generator，选择copy all used libraries into project folder
![Alt](https://imgconvert.csdnimg.cn/aHR0cHM6Ly90dHR0dHR0dHR0dHR0dC5vc3MtY24tYmVpamluZy5hbGl5dW5jcy5jb20vY2xpb24vJUU5JTgwJTg5JUU2JThCJUE5JUU2JTg5JTgwJUU2JTlDJTg5JUU2JTk2JTg3JUU0JUJCJUI2LnBuZw?x-oss-process=image/format,png)
然后
## 2.创建工程
按照普通的方法创建clion和stm32cube的工程，然后导入DSP库

编译器的面板配置文件选用stm32f7discovery，不要使用eval。

### 下载的时候一定要打开板子电源！！！
### 下载的时候一定要打开板子电源！！！
### 下载的时候一定要打开板子电源！！！
要不然下载失败可能就当选错配置文件了。
