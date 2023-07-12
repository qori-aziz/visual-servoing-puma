from sklearn.metrics import mean_squared_error
  
# # Given values
# Y_true = [1,1,2,2,4]  # Y_true = Y (original values)
  
# # calculated values
# Y_pred = [0.6,1.29,1.99,2.69,3.4]  # Y_pred = Y'
  
# # Calculation of Mean Squared Error (MSE)
# print(mean_squared_error(Y_true,Y_pred))

import csv

# Process ground truth
uDict = {}
vDict = {}
with open('yoloresults/groundtruth.csv', mode='r') as infile:
    reader = csv.reader(infile)
    next(reader, None)  # skip the headers
    for rows in reader:
        k = rows[0]
        u = float(rows[3])
        v = float(rows[4])
        # print(k,v)
        uDict[k] = u
        vDict[k] = v
    # print(uDict)

print("next")
# Process YOLOV5medium
uYoloDict = {}
vYoloDict = {}
with open('yoloresults/YoloV7XL.csv', mode='r') as infile:
    reader = csv.reader(infile)
    next(reader, None)  # skip the headers
    for rows in reader:
        k = rows[0]
        u = float(rows[3])
        v = float(rows[4])
        # print(k,v)
        uYoloDict[k] = u
        vYoloDict[k] = v
    # print(uYoloDict)

# Iterate from ground truth
Y_true = list()
U_true = list()
V_true = list()

Y_pred= list()
U_pred = list()
V_pred = list()

for key in uYoloDict:
    U_true.append(uDict[key])
    U_pred.append(uYoloDict.get(key, 0))

    total = [uDict[key], vDict[key]]
    Y_true.append(total)

    totalPred = [uYoloDict.get(key, 0), vYoloDict.get(key, 0)]
    Y_pred.append(total)

for key in vYoloDict:
    V_true.append(vDict[key])
    V_pred.append(vYoloDict.get(key, 0))

print(Y_pred)
print(Y_true)

print("U mse: ", mean_squared_error(U_true,U_pred))
print("V mse: ", mean_squared_error(V_true,V_pred))
print("Total MSE: ", mean_squared_error(Y_true, Y_pred))