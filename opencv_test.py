import cv2
import pixy2_raw_frame as p
from PIL import Image
import yolov7_new.detect as d
import time
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
while True:
        t0 = time.time()
        p.getFrame()

        # Convert ppm to jpg
        im = Image.open("out0.ppm")
        im.save("yolov7/out.jpg")

        err_vertical, err_horizontal = d.detect(model)
        client.publish("errver", err_vertical)
        client.publish("errhor", err_horizontal)

        t1 = time.time()

        # image = cv2.imread("yolov7/out.jpg")
        # cv2.imshow("Image", image)
        # cv2.waitKey(1)
        print(f'Waktu 1 frame: ({(1E3 * (t1 - t0)):.1f}ms)')