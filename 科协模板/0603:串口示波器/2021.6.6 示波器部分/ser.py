import time
import serial
import pyqtgraph as pg
import numpy as np
from PyQt5.Qt import QThread

buff = []
buff_size = 10
ptr = 0


# **********************************************************************************************************************#
# 解码程序, 你需要解码你发过来的数据
# def data_decode(data_buff):
#     # add code begin
#     global ptr
#     data = buff[ptr]
#     temp = ''
#     for i in range(buff_size):
#         temp += buff[i]
#     try:
#         data = data.split(',')
#         data = [int(i) for i in data]
#     return buff
#     # add code end
def data_decode():
    temp = ''
    for i in buff:
        temp += i
    data = temp.split('\r')
    # print(data)
    data = [eval(i) for i in data if len(i) == 4 or len(i) == 5]
    return data


def draw():
    def plot_data(curve):
        """
        :param curve: curve是一个子图的.plot()
        eg.在MyGraphWindows中，可以用p1.plot()作参数
        :return:
        """
        data = data_decode()
        curve.setData(np.array(data))

    timer = pg.QtCore.QTimer()
    timer.timeout.connect(plot_data)  # 定时调用plotData函数
    timer.start(1000)  # 多少ms调用一次


# **********************************************************************************************************************#
def serial_receive(com):
    while 1:
        try:
            seri = serial.Serial(com, 115200)  # windows系统使用com1口连接串行口
            print("L")
        except Exception as e:
            print(".")
            time.sleep(0.5)
            continue
        break
    seri.parity = serial.PARITY_NONE  # 校验位
    if seri.isOpen():
        seri.close()
    seri.open()
    print("Success config a Serial")
    read_buff = seri.read(1)
    print(read_buff)
    while read_buff.decode() != "\r":
        read_buff = seri.read(1)
        print(read_buff)
    while True:
        read_buff = seri.read(500)
        # 暂定输入格式为"0000\r0000\r0000..."
        # 每个数必须是4位
        # 用一个换行符分隔，每次发送100个数据
        if len(buff) <= buff_size:
            buff.append(read_buff.decode('utf-8'))
        else:
            buff[:-1] = buff[1:]
            buff[-1] = read_buff.decode('utf-8')
        # print(buff)


class User_Thread(QThread):
    com = None

    def __init__(self):
        super(User_Thread, self).__init__()

    def getCom(self, userInput):
        self.com = userInput

    def run(self):
        serial_receive(self.com)
