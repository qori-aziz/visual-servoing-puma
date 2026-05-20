import csv
import numpy as np


with open('data_30cm.csv') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    # print(csv_reader)
    line_count = 0
    lst = list(csv_reader)
    array = np.array(lst)
    # print(array)
    with open('data_30cm_new.csv', 'w', encoding='UTF8') as f:
        writer = csv.writer(f)
        for a in array:
            arr = np.array([a[0]])
            x = (np.char.split(arr))
            # print(x[0][1])
            data = (x[0][1],x[0][2],x[0][3],x[0][4])
            writer.writerow(data)
        
    #     for a in array:
    #         arr = np.array([a[0]])
    #         x = np.char.split(arr)
    #         data = (x[1],x[2],x[3],x[4])
    #         writer.writerow(data)
    #         # for a in x:
            #     print(a)
    # print(array[5][0])
    # print(array[5])
    # print(type(array[5][0]))
    # c = array[5][0].astype(np.float_)
    # print(np.char.split(array[5][0]))
    
    # x = np.char.split(arr)
    # # print(np.concatenate(x))
    # # print(type(x))
    # for a in x:
    #     print(a)
    # print("Array Dimension = ",len(x.shape))
    # print(x.shape)
    # tes = np.fromstring(array[0][0], dtype=float, sep='   ')
    # print(tes)
    # for n in lst[0]:
    #     array = np.fromstring(lst[0], dtype=float, sep='         ')
    #     print(array)
    # for row in csv_reader:
    #     print(row[0])
    # print(f'Processed {line_count} lines.')