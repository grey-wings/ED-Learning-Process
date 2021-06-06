from PyQt5 import QtCore, QtGui, QtWidgets
import serial.tools.list_ports
import numpy as np
import pyqtgraph as pg
import mainGUI
import ser


class MyGraphWindows(QtWidgets.QDialog, mainGUI.Ui_Dialog):
    com = 'COM1'
    win = None
    p1, p2 = None, None
    timer = None

    def __init__(self, parent=None):
        super(MyGraphWindows, self).__init__(parent)
        self.x = []
        self.y = []
        self.setupUi(self)  # 初始化窗口
        self.p1, self.p2 = self.set_graph_ui()  # 设置绘图窗口
        self.ser_thread = ser.User_Thread()

        # self.comSelectBox.addItems(["COM" + str(i) for i in range(1, 7)])
        self.port_list = list(serial.tools.list_ports.comports())
        # 寻找当前可用的串口
        self.comSelectBox.addItems([i[0] for i in self.port_list])
        # i[0]代表串口号，如'COM1', 'COM2'
        self.com = self.port_list[0][0]
        self.ser_thread.getCom(self.com)

        self.comSelectBox.activated[str].connect(self.setCOM)
        self.beginButton.clicked.connect(self.beginEvent)

    def set_graph_ui(self):
        pg.setConfigOptions(antialias=True)
        self.win = pg.GraphicsLayoutWidget()  # 创建pg layout?

        self.graph_layout.addWidget(self.win)

        self.p1 = self.win.addPlot(title="波形图")
        self.p1.setLabel('left', text='meg', color='#ffffff')  # y轴设置函数
        self.p1.addLegend()

        self.win.nextRow()  # layout换行，采用垂直排列，不添加此行则默认水平排列
        self.p2 = self.win.addPlot(title="频谱")
        self.p2.setLabel('left', text='meg', color='#ffffff')  # y轴设置函数
        self.p2.addLegend()
        self.p1 = self.p1.plot()
        self.p2 = self.p2.plot()
        return self.p1, self.p2

    def setCOM(self):
        self.com = self.comSelectBox.currentText()
        self.ser_thread.getCom(self.com)

    #
    # def draw(self):
    #     x = np.arange(0, 9, 0.05)
    #     y = np.arange(0, 9, 0.05)
    #     self.p1.plot(x, y)
    def draw_data_start(self):
        def plot_data():
            data = ser.data_decode()
            self.p1.setData(np.array(data))

        # curve.setPos(ptr, 0)

        self.timer = pg.QtCore.QTimer()
        self.timer.timeout.connect(plot_data)  # 定时调用plotData函数
        self.timer.start(1000)  # 多少ms调用一次

    def beginEvent(self):
        if self.beginButton.text() == '开始':
            self.beginButton.setText('停止')
            # self.ser_threading = threading.Thread(target=ser.serial_receive(self.com))
            # self.ser_threading.start()
            self.ser_thread.start()
            self.draw_data_start()

        else:
            self.beginButton.setText('开始')
            self.timer.stop()  # 多少ms调用一次
