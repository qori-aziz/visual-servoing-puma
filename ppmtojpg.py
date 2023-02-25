from PIL import Image

im = Image.open("yolov7/out0.ppm")
im.save("yolov7/out0.jpg")