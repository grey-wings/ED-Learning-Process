import cv2
import numpy as np
import matplotlib.pyplot as plt

# 说明：
# 1. cv2采用BGR模式，matplotlib采用RGB模式，用plt输出会使颜色错误
# 2. imshow使用的窗口名字不能有空格

# 一些处理方法：
# 中值滤波: result = cv2.medianBlur(image, 5)
#
# 图像梯度: dst = cv2.Laplacian(src, ddepth, dst, ksize, scale, delta, borderType)
# 前两个参数是必须的，第一个是要处理的图像，第二个参数是图像的深度，
# -1表示采用的是与原图像相同的深度。目标图像的深度必须大于等于原图像的深度；
#
# 霍夫圆环检测:
# cv2.HoughCircles(image, method, dp, minDist, circles=None, param1=None, param2=None, minRadius=None, maxRadius=None)
# image:8位，单通道图像。如果使用彩色图像，需要先转换为灰度图像。
# method：定义检测图像中圆的方法。目前唯一实现的方法是cv2.HOUGH_GRADIENT。
# dp：累加器分辨率与图像分辨率的反比。dp获取越大，累加器数组越小。
# minDist：检测到的圆的中心，（x,y）坐标之间的最小距离。如果minDist太小，
# 则可能导致检测到多个相邻的圆。如果minDist太大，则可能导致很多圆检测不到。

# pic_diff = cv2.absdiff(pic1, pic2)
# 它可以把两幅图的差的绝对值输出到另一幅图上面来


def Show_Picture(image, SaveName="output.png"):
    cv2.imwrite(SaveName, image)
    # 保存图片，第一个参数为名字，第二个参数为图片
    cv2.namedWindow(SaveName, cv2.WINDOW_NORMAL)
    # namedWindow设置窗口属性，这里是用户可以拖动调整窗口大小
    cv2.imshow(SaveName, image)
    # imshow第一个参数是显示窗口名称，第二个参数是要显示的图片
    cv2.waitKey()
    cv2.destroyAllWindows()
    # 上面两句话让生成的图片窗口不闪退


def Generate_Gray_Photo(FilePath):
    image_Gray = cv2.imread(FilePath, 0)  #后面一个参数为1时，是RGB模式,默认值
                                     #为0时，灰度模式
                                     #为-1时，包括alpha
    return image_Gray


def Generate_BGR_Photo(FilePath):
    image_BGR = cv2.imread(FilePath, 1)  # 后面一个参数为1时，是RGB模式,默认值
                                         # 为0时，灰度模式
                                         # 为-1时，包括alpha
    return image_BGR


def Generate_HSV_Photo(FilePath):
    image = cv2.imread(FilePath, 1)  #后面一个参数为1时，是RGB模式,默认值
                                     #为0时，灰度模式
                                     #为-1时，包括alpha
    image_HSV = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    return image_HSV


def Transfer_16bit_to_8bit(image_16bit):
    min_16bit = np.min(image_16bit)
    max_16bit = np.max(image_16bit)
    # image_8bit = np.array(np.rint((255.0 * (image_16bit - min_16bit)) / float(max_16bit - min_16bit)), dtype=np.uint8)
    # 或者下面一种写法
    image_8bit = np.array(np.rint(255 * ((image_16bit - min_16bit) / (max_16bit - min_16bit))), dtype=np.uint8)
    return image_8bit


def Cut_Color(image, colour):
    '''image是要处理的图片，colour是包括颜色上限和下限的元组'''
    mask = cv2.inRange(image, colour[0], colour[1])
    #为HSV图像设置阈值，获得一种颜色的图像
    res = cv2.bitwise_and(image, image, mask=mask)
    # 图像位运算
    # 利用掩膜（mask）进行“与”操作，
    # 即掩膜图像白色区域是对需要处理图像像素的保留，
    # 黑色区域是对需要处理图像像素的剔除
    return res


def Gaussian_and_Orst(image):
    '''对图像进行高斯滤波和Orst阈值处理'''
    image = Transfer_16bit_to_8bit(image)
    blur = cv2.GaussianBlur(image, (5, 5), 0)
    #对image进行5*5高斯滤波
    ret, dst = cv2.threshold(blur, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)
    #ret是阈值，dst是Orst阈值处理后的图像。
    return ret, dst


def Hough(image, image_gray):
    '''生成图中圆的圆心和轮廓
       输入为一个BGR图像'''
    circles = cv2.HoughCircles(image_gray, cv2.HOUGH_GRADIENT, 1, 300,
    param1=50, param2=30, minRadius=140, maxRadius=300)
    # image: 8位，单通道图像。如果使用彩色图像，需要先转换为灰度图像。
    # method：定义检测图像中圆的方法。目前唯一实现的方法是cv2.HOUGH_GRADIENT。
    # dp：累加器分辨率与图像分辨率的反比。dp获取越大，累加器数组越小。
    # minDist：检测到的圆的中心，（x, y）坐标之间的最小距离。如果minDist太小，则可能导致检测到多个相邻的圆。
    # 如果minDist太大，则可能导致很多圆检测不到。
    # param1：用于处理边缘检测的梯度值方法。
    # param2：cv2.HOUGH_GRADIENT方法的累加器阈值。阈值越小，检测到的圈子越多。
    # minRadius：半径的最小大小（以像素为单位）。
    # maxRadius：半径的最大大小（以像素为单位）。
    circles = np.uint16(np.around(circles))
    for i in circles[0, :]:
        # draw the outer circle
        cv2.circle(image, (i[0], i[1]), i[2], (0, 255, 0), 2)
        # draw the center of the circle
        cv2.circle(image, (i[0], i[1]), 2, (0, 0, 255), 3)
    return image


def Maximum_Connected_domain(image_binary):
    """
    本函数返回一个函数的最大连通域
    :param image_binary: 一个二值化图像
    :return: image_binary: 处理后的图像
             coutours: 轮廓
    """
    # 轮廓
    contours, hierarchy = cv2.findContours(image_binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    area = []
    for i in range(len(contours)):
        area.append(cv2.contourArea(contours[i]))
    max_idx = np.argmax(area)
    for i in range(len(contours)):
        if i != max_idx:
            cv2.fillConvexPoly(image_binary, contours[i], 0)
    cv2.fillConvexPoly(image_binary, contours[max_idx], 255)
    return image_binary, contours


def Min_Enclosing_Circle(mask, original_image):
    """
    对mask求最大连通域，然后在original_image上面画圆
    :param mask: 没什么好说的
    :param original_image: 最初始的图像（要在这个图像上面画圆）
    :return: 第一个是处理后的mask，第二个是处理后的初始图像
    """
    mask, contours1 = Maximum_Connected_domain(mask)
    (x, y), radius = cv2.minEnclosingCircle(contours1[0])
    # 和boundingRect不同（输入参数可以是图像和点集），minEnclosingCircle的输入只能是点集(contours[0])，否则报错
    # 报错信息是 ...(depth == CV_32F || depth == CV_32S) in function 'cv::minEnclosingCircle'
    # 在网上找很容易被当成opencv版本问题（因为opencv3的findContours返回的hierarchy和contours位置是反的
    center = (int(x), int(y))
    radius = int(radius)
    cv2.circle(original_image, center, radius, (0, 0, 255), 2)
    cv2.circle(original_image, center, 2, (0, 255, 0), 2)

    return mask, original_image


def Catch_Ball(image):
    lower = np.array([9, 0, 0])
    upper = np.array([38, 255, 66])
    blurimg = cv2.GaussianBlur(image, (5, 5), 0)
    hsv_image = cv2.cvtColor(blurimg, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv_image, lower, upper)

    kernal = cv2.getStructuringElement(cv2.MORPH_RECT, (12, 12))
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernal)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernal)
    # Show_Picture(mask)

    Maximum_Connected_domain(mask)
    # Show_Picture(mask)

    # close->先腐蚀再膨胀 open->先膨胀再腐蚀
    x, y, w, h = cv2.boundingRect(mask)
    # 最小外接圆 x,y是坐标，radius是半径
    cv2.rectangle(image, (x, y), (x+w, y+h), (0, 0, 255), 2)
    return image


def trace(FilePath, SavePath="T1_outcome.avi"):
    video = cv2.VideoCapture(FilePath)
    firstframe = None
    num = 0
    # 获取视频宽高
    frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    out = cv2.VideoWriter(SavePath, cv2.VideoWriter_fourcc(*'XVID'), 20, (frame_width, frame_height))
    # 第一个参数表示要生成的文件名称，第三个是帧数，第四个是width和height的元组
    while True:
        ret, frame = video.read()
        # 第一个参数ret为True或者False, 代表有没有读取到图片
        # 第二个参数frame表示截取到一帧的图片
        if not ret:
            break

        Catch_Ball(frame)

        out.write(frame)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
        num += 1
        print("当前帧数：", num)
    out.release()
    video.release()
    cv2.destroyAllWindows()





def Solve2(FilePath, SavePath="T2_outcome.avi"):
    video = cv2.VideoCapture(FilePath)
    firstframe = None
    num = 0
    lower = np.array([9, 0, 0])
    upper = np.array([38, 255, 66])
    # 获取视频宽高
    frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    out = cv2.VideoWriter(SavePath, cv2.VideoWriter_fourcc(*'XVID'), 20, (frame_width, frame_height))
    # 第一个参数表示要生成的文件名称，第三个是帧数，第四个是width和height的元组
    # while True:
    for i in range(2):
        ret, frame = video.read()
        # 第一个参数ret为True或者False, 代表有没有读取到图片
        # 第二个参数frame表示截取到一帧的图片
        if not ret:
            break
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        _, frame = cv2.threshold(frame, thresh=127, maxval=255, type=cv2.THRESH_BINARY)
        # 阈值函数，第一个返回值是阈值（省略），第二个返回值是图像
        # 第一个参数是图像，这里采用二值化type，值>thresh的像素点将被设置为maxval

        out.write(frame)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
        num += 1
        print("当前帧数：", num)
    out.release()
    video.release()
    cv2.destroyAllWindows()


def try0130(FilePath, SavePath='20210130_first_try.avi'):
    '''
    创建于2021年1月30日的处理函数
    基于camshift
    :param FilePath: 要处理的视频路径
    :param SavePath: 结果保存路径
    :return:
    '''
    # lower = np.array([9, 0, 0])
    # upper = np.array([38, 255, 66])
    ll = np.array([28/2, 35*255/100, 80*255/100])
    hh = np.array([40/2, 90*255/100, 100*255/100])
    video = cv2.VideoCapture(FilePath)
    firstframe = None
    num = 0
    # 获取视频宽高
    fps = int(video.get(cv2.CAP_PROP_FPS))
    frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    out = cv2.VideoWriter(SavePath, cv2.VideoWriter_fourcc(*'XVID'), fps, (frame_width, frame_height), True)
    # 第一个参数表示要生成的文件名称，第三个是帧数，第四个是width和height的元组
    # 最后一个参数(isColor)是True则说明是彩色图像，False是黑白图像
    while True:
        ret, frame = video.read()
        # 第一个参数ret为True或者False, 代表有没有读取到图片
        # 第二个参数frame表示截取到一帧的图片
        if not ret:
            break
        # 基础操作：滤波、转换颜色和颜色选择
        blurimg = cv2.GaussianBlur(frame, (5, 5), 0)
        hsv_image = cv2.cvtColor(blurimg, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(hsv_image, ll, hh)
        # 修正选区
        kernal = cv2.getStructuringElement(cv2.MORPH_RECT, (12, 12))
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernal)
        mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernal)
        # 画圆
        _, image = Min_Enclosing_Circle(mask, frame)

        out.write(image)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
        num += 1
        print("当前帧数：", num)
    out.release()
    video.release()
    cv2.destroyAllWindows()


try0130("VID_20210131_140619.mp4")
