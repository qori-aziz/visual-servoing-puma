import os
import glob
import pandas as pd
import numpy as np


path = 'YoloV7XL'
os.chdir(r'yoloresults/YoloV7XL/labels')
# path = 'Purple Ball.v1-v1.0.0.yolov7pytorch/test/labels'
myFiles = glob.glob('*.txt')

# for file in glob.glob( "*.txt"):
#     print(file)

# f = open("conv_out3_jpg.rf.e04d1d12097a46d58faf27836a27ab61.txt", "r")
# print(f.read()) 

width=320
height=200
image_id=0
final_df=[]
for item in myFiles:
    row=[]
    bbox_temp=[]
    image_id=item
    print(image_id)
    with open(item, 'rt') as fd:
        for line in fd.readlines():
           splited = line.split()
           row.append(image_id)
           row.append(width)
           row.append(height)
           if len(splited) != 0:
                row.append(float(splited[1])*width)
                row.append(float(splited[2])*height)
           final_df.append(row)
        #    try:
        #        bbox_temp.append(float(splited[1])*width)
        #        bbox_temp.append(float(splited[2])*height)
        #        bbox_temp.append(float(splited[3])*width)
        #        bbox_temp.append(float(splited[4])*height)
        #        row.append(bbox_temp)
        #        final_df.append(row)
        #    except:
        #        print("file is not in YOLO format!")
            
df = pd.DataFrame(final_df,columns=['image_id', 'width', 'height','u_center', 'v_center'])
df.to_csv(path + ".csv",index=False)

# Process unprocessed txt
# os.chdir(r'yoloresults/YoloV5medium/labels')
# with open("conv_out3_jpg.rf.e04d1d12097a46d58faf27836a27ab61.txt", 'rt') as fd:
#     for line in fd.readlines():
#         splited = line.split()
#         if len(splited) != 0:
#             print(float(splited[1])*width)
#             print(float(splited[2])*height)