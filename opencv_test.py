import cv2
import pixy2_raw_frame as p
from PIL import Image
# import yolov5_new.detect as d
import yolov7_new.detect as d
import time
import csv
import paho.mqtt.client as mqtt
import numpy as np
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures

# Set pixycam
res = p.initializing()
if res != 0:
        print(res)
        quit()
## Set YOLO
model = d.initialization()

## Set MQTT
Id = "producerPy"
Ip = "localhost"
client = mqtt.Client(Id)
client.connect(Ip, 1883, 60)
f = open('errdata.csv', 'w')
# writer = csv.writer(f)

# Desired feature : 25.26,83.0,25.0,257.0,193.0 (D, u1,v1,u4,v4)

## Set depth estimation
x = np.array([
    [206.2868217],
    [112.2826087],
    [104.0526316],
    [84.86111111],
    [83.32624113],
    [56.99367089],
    [66.32704403],
    [55.83707865],
    [47.47619048],
])

y = np.array([50, 80, 100, 130, 150, 180, 200, 250, 300])

modelLinear = LinearRegression()
modelLinear.fit(x,y)

x_ = PolynomialFeatures(degree=2, include_bias=False).fit_transform(x)
modelQuadratic = LinearRegression().fit(x_, y)

try:
    while True:
        t0 = time.time()
        p.getFrame()

        # Convert ppm to jpg
        # YOLOv7
        im = Image.open("out0.ppm")
        im.save("yolov7_new/out.jpg")

        # #YOLOv5
        # im = Image.open("out0.ppm")
        # im.save("yolov5_new/out.jpg")

        na, inference_time, nms_time = d.detect(model)
        # image = cv2.imread("yolov7/out.jpg")
        # cv2.imshow("Image", image)
        # cv2.waitKey(1)
        depth = 0
        u_lefttop = 0
        v_lefttop = 0
        u_rightdown = 0
        v_rightdown = 0
        if len(na)>0:
            u_lefttop = na[0][0]
            v_lefttop = na[0][1]
            u_rightdown = na[0][2]
            v_rightdown = na[0][3]
            width = abs(na[0][0] - na[0][2])
            # length = abs(na[0][1] - na[0][3])
            dataDepth = np.array([width])
            print(dataDepth)
            y_pred_linear = modelLinear.predict(np.array(dataDepth).reshape(1, -1))
            x_ = PolynomialFeatures(degree=2, include_bias=False).fit_transform(dataDepth.reshape(1, -1))
            y_pred_quadratic = modelQuadratic.predict(x_.reshape(1, -1))
            print(f"predicted linear response:{y_pred_linear} \n")
            print(f"predicted quadratic response:{y_pred_quadratic} \n")
            depth = y_pred_quadratic[0]
            # data = (width,length)
            # writer.writerow(data)
            data = f'{depth:.2f},{u_lefttop},{v_lefttop},{u_rightdown},{v_rightdown}'
            client.publish("data", data)        
        t1 = time.time()
        one_frame_time = 1E3 * (t1 - t0)
        print(f'Total 1 frame time: ({one_frame_time:.1f}ms)')
except KeyboardInterrupt:
    f.close()
    pass