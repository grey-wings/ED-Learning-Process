import cv2
import numpy as np
import queue

up = True
down = False
left = True
right = False
direction = None


def Show_Picture(image, SaveName="output.png"):
    """
    显示和保存图片的函数。
    :param image: 要显示和保存的图片。
    :param SaveName: 保存的路径或名字。
    :return: 无
    """
    cv2.imwrite(SaveName, image)
    # 保存图片，第一个参数为名字，第二个参数为图片
    cv2.namedWindow(SaveName, cv2.WINDOW_NORMAL)
    # namedWindow设置窗口属性，这里是用户可以拖动调整窗口大小
    cv2.imshow(SaveName, image)
    # imshow第一个参数是显示窗口名称，第二个参数是要显示的图片
    cv2.waitKey()
    cv2.destroyAllWindows()
    # 上面两句话让生成的图片窗口不闪退


def Show_Direction_UD(image, direction):
    """
    绘制上下方向箭头。
    :param image:要画箭头的图像。
    :param direction:方向;上为true,下为false.
    :return: 画完箭头的图像。
    """
    x = 200
    y1 = 200
    y2 = 500
    if direction:
        cv2.arrowedLine(image, (x, y2), (x, y1), (0, 0, 255),
                        thickness=6, line_type=cv2.LINE_4, shift=0, tipLength=0.1)
    else:
        cv2.arrowedLine(image, (x, y1), (x, y2), (0, 0, 255),
                        thickness=6, line_type=cv2.LINE_4, shift=0, tipLength=0.1)
    return image


def Show_Direction_LR(image, direction):
    """
    绘制左右方向箭头。
    :param image:要画箭头的图像
    :param direction:方向;左为true,右为false.
    :return:画完箭头的图像。
    """
    x1 = 200
    x2 = 500
    y = 200
    if direction:
        cv2.arrowedLine(image, (x1, y), (x2, y), (0, 0, 255),
                        thickness=6, line_type=cv2.LINE_4, shift=0, tipLength=0.1)
    else:
        cv2.arrowedLine(image, (x2, y), (x1, y), (0, 0, 255),
                        thickness=6, line_type=cv2.LINE_4, shift=0, tipLength=0.1)
    return image


def Draw_Point(image, center):
    """
    给出圆心，画出点和圆心。
    :param image: 要画圆的图像。
    :param center: 圆心。
    :return: 画完圆的图像。
    """
    cv2.circle(image, center, 20, (0, 255, 0), 2)
    cv2.circle(image, center, 2, (0, 0, 255), 2)  # 用极小半径的圆代替点
    return image


def Generate_Gray_Photo(FilePath):
    """
    从特定图片路径生成灰度图像的函数。
    :param FilePath: 要生成图像的路径。
    :return: 生成的灰度图像。
    """
    image_Gray = cv2.imread(FilePath, 0)  # 后面一个参数为1时，是RGB模式,默认值
                                          # 为0时，灰度模式
                                          # 为-1时，包括alpha
    return image_Gray


def Generate_BGR_Photo(FilePath):
    """
    从特定图片路径生成BGR图像的函数。
    :param FilePath: 要生成图像的路径。
    :return: 生成的BGR图像。
    """
    image_BGR = cv2.imread(FilePath, 1)  # 后面一个参数为1时，是RGB模式,默认值
    # 为0时，灰度模式
    # 为-1时，包括alpha
    return image_BGR


def Transfer_16bit_to_8bit(image_16bit):
    """
    将16位图像转为8位图像的函数。
    :param image_16bit: 16位的图像。
    :return: 转换完成的8位图像。
    """
    min_16bit = np.min(image_16bit)
    max_16bit = np.max(image_16bit)
    # image_8bit = np.array(np.rint((255.0 * (image_16bit - min_16bit)) / float(max_16bit - min_16bit)), dtype=np.uint8)
    # 或者下面一种写法
    image_8bit = np.array(np.rint(255 * ((image_16bit - min_16bit) / (max_16bit - min_16bit))), dtype=np.uint8)
    return image_8bit


def Gaussian_and_Orst(image):
    """
    对图像进行高斯滤波和Orst阈值处理
    :param image:
    :return: 处理后的图片
    """
    image = Transfer_16bit_to_8bit(image)
    blur = cv2.GaussianBlur(image, (5, 5), 0)
    # 对image进行5*5高斯滤波
    ret, dst = cv2.threshold(blur, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    # ret是阈值，dst是Orst阈值处理后的图像。
    return dst


def Binarization(image):
    """
    将输入图像(BGR)二值化和腐蚀。
    :param image: 要进行处理的图像。
    :return: 处理完毕的图像。
    """
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    dst = Gaussian_and_Orst(image)
    kernel = np.ones((5, 5), np.uint8)
    erosion = cv2.erode(dst, kernel, iterations=1)
    return erosion


def try_Houghline(raw_image, binary_image):
    """
    基于Houghline的尝试
    创建于腊月卄五
    :param raw_image: 原始图像
    :param binary_image: 二值化图像
    :return:
    """
    lines = cv2.HoughLinesP(binary_image, 1, np.pi / 180, 100, minLineLength=200, maxLineGap=50)
    for line in lines:
        x1, y1, x2, y2 = line[0]
        cv2.line(raw_image, (x1, y1), (x2, y2), (0, 255, 0), 2)
    return raw_image


def Solve2(FilePath, SavePath="T2_outcome.avi"):
    video = cv2.VideoCapture(FilePath)
    firstframe = None
    num = 0
    lower = np.array([9, 0, 0])
    upper = np.array([38, 255, 66])
    preview_lst = []
    # 获取视频宽高
    frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    out = cv2.VideoWriter(SavePath, cv2.VideoWriter_fourcc(*'XVID'), 20, (frame_width, frame_height), False)
    # 第一个参数表示要生成的文件名称，第三个是帧数，第四个是width和height的元组
    # while True:
    while True:
        ret, frame = video.read()
        # 第一个参数ret为True或者False, 代表有没有读取到图片
        # 第二个参数frame表示截取到一帧的图片
        if not ret:
            break
        img = Binarization(frame)
        img = cv2.Canny(img, 10, 12)
        lst = list(img[:, 0])
        diff_lst = []
        if preview_lst:
            for i in range(len(lst)):
                diff_lst.append(lst[i] - preview_lst[i])

        print(diff_lst)
        preview_lst = lst

        out.write(img)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
        num += 1
        # print("当前帧数：", num)
    out.release()
    video.release()
    cv2.destroyAllWindows()


def Try_Detect_Corner_Points(img):
    """
    通过goodFeaturesToTrack算出角点
    取平均值
    检测速度方向
    :param image: 输入的图片
    :return:
    """
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    _, img_gray = Gaussian_and_Orst(img_gray)

    # 检测角点
    feature_params = dict(maxCorners=100, qualityLevel=0.3, minDistance=7, blockSize=7)
    p0 = cv2.goodFeaturesToTrack(img_gray, mask=None, **feature_params)
    x_avg, y_avg = 0, 0
    # 计算角点平均值
    if p0 is not None:
        x_sum = 0
        y_sum = 0
        for i, old in enumerate(p0):
            x, y = old.ravel()
            x_sum += x
            y_sum += y
        x_avg = int(x_sum / len(p0))
        y_avg = int(y_sum / len(p0))
    cv2.circle(img, (x_avg, y_avg), 20, (0, 0, 255), 2)
    cv2.circle(img, (x_avg, y_avg), 2, (0, 255, 0), 2)

    return img


def Try2(FilePath, SavePath="T2_outcome.avi"):
    """
    基于特征点队列的函数
    :param FilePath:
    :param SavePath:
    :return:
    """
    video = cv2.VideoCapture(FilePath)
    previewFrame = None
    num = 0

    feature_params = dict(maxCorners=100,
                          qualityLevel=0.3,
                          minDistance=7,
                          blockSize=7)
    # 获取视频宽高
    frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    out = cv2.VideoWriter(SavePath, cv2.VideoWriter_fourcc(*'XVID'), 20, (frame_width, frame_height))
    # 第一个参数表示要生成的文件名称，第三个是帧数，第四个是width和height的元组
    q_x = queue.Queue(maxsize=10)
    q_y = queue.Queue(maxsize=10)
    x_avg, y_avg = 0, 0

    while True:
        ret, frame = video.read()
        # 第一个参数ret为True或者Falsm
        #
        # ccccce, 代表有没有读取到图片
        # 第二个参数frame表示截取到一帧的图片
        if not ret:
            break

        img_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        img_gray = Gaussian_and_Orst(img_gray)
        if previewFrame is None:
            previewFrame = img_gray
            continue

        diff = cv2.absdiff(previewFrame, img_gray)
        p0 = cv2.goodFeaturesToTrack(diff, mask=None, **feature_params)

        if p0 is not None:
            x_sum = 0
            y_sum = 0
            for i, old in enumerate(p0):
                x, y = old.ravel()
                x_sum += x
                y_sum += y
            x_avg = int(x_sum / len(p0))
            y_avg = int(y_sum / len(p0))
            Draw_Point(frame, (x_avg, y_avg))

            if q_y.full():
                qy_list = list(q_y.queue)
                key = 0
                diffy_sum = 0
                for item_y in qy_list:
                    key += 1
                    if key < 10:
                        diff_y = item_y - qy_list[key]
                        diffy_sum += diff_y
                if diffy_sum < 0:
                    Show_Direction(frame, up)
                else:
                    Show_Direction(frame, down)
                q_y.get()  # 对空队列get会卡死
        q_y.put(y_avg)
        previewFrame = img_gray
        out.write(frame)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
        num += 1
        print("当前帧数：", num)
    out.release()
    video.release()
    cv2.destroyAllWindows()


def Solve20210219(FilePath, SavePath="20210219.avi"):
    """
    首次完成于2021年2月19日
    基于phaseCorrelate的方向检测
    仅支持上下运动
    基本实现题目基础要求
    部分情况下方向检测不准确
    :param FilePath: 输入文件的路径
    :param SavePath: 输出文件保存的路径
    :return:
    """
    global direction
    video = cv2.VideoCapture(FilePath)
    previewFrame = None
    num = 0

    feature_params = dict(maxCorners=100, qualityLevel=0.3,
                          minDistance=7, blockSize=7)
    # 获取视频宽高
    frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    out = cv2.VideoWriter(SavePath, cv2.VideoWriter_fourcc(*'XVID'),
                          20, (frame_width, frame_height), True)
    # 第一个参数表示要生成的文件名称，第三个是帧数，第四个是width和height的元组
    while True:
        ret, frame = video.read()
        # 第一个参数ret为True或者Falsm
        #
        # ccccce, 代表有没有读取到图片
        # 第二个参数frame表示截取到一帧的图片
        if not ret:
            break

        img_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        img_gray = Gaussian_and_Orst(img_gray)

        img_gray = img_gray.astype(float)
        if previewFrame is None:
            previewFrame = img_gray
            continue

        movement, _ = cv2.phaseCorrelate(previewFrame, img_gray)
        print(movement)
        move = movement[1]

        # 消抖，避免了部分情况下的方向乱跳的情况
        if 0 < move < 15:
            direction = up
        elif -15 < move < 0:
            direction = down
        else:
            pass

        Show_Direction_UD(frame, direction)
        previewFrame = img_gray  # 千万不要忘记这一步！！！
        out.write(frame)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
        num += 1
        print("当前帧数：", num)
    out.release()
    video.release()
    cv2.destroyAllWindows()


Solve20210219("try1.mp4")
