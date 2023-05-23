from PIL import Image

im = Image.open("yolov7_new/out0.ppm")
im.save("yolov7_new/out0.jpg")