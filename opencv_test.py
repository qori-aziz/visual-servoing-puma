import cv2
import pixy2_raw_frame as p
from PIL import Image
import yolov5_new.detect as d
# import yolov7_new.detect as d
import time
import csv
import paho.mqtt.client as mqtt

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
f = open('timedata.csv', 'w')
writer = csv.writer(f)

try:
    while True:
        t0 = time.time()
        p.getFrame()

        # Convert ppm to jpg
        #YOLOv7
        # im = Image.open("out0.ppm")
        # im.save("yolov7_new/out.jpg")

        #YOLOv5
        im = Image.open("out0.ppm")
        im.save("yolov5_new/out.jpg")

        err_vertical, err_horizontal, inference_time, nms_time = d.detect(model)
        client.publish("errver", err_vertical)
        client.publish("errhor", err_horizontal)

        t1 = time.time()

        # image = cv2.imread("yolov7/out.jpg")
        # cv2.imshow("Image", image)
        # cv2.waitKey(1)
        
        
        one_frame_time = 1E3 * (t1 - t0)
        data = (inference_time, nms_time, one_frame_time)

        writer.writerow(data)
        
        print(f'Total 1 frame time: ({one_frame_time:.1f}ms)')
except KeyboardInterrupt:
    f.close()
    pass