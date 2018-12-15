import matplotlib.pyplot as plt
from PIL import Image
import numpy as np
import cv2
import random

oriimg = np.array(Image.open('pic.png'))
img = oriimg.copy()

def randcolor():
    return (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255), 255)

inf = open('path.c', 'r',encoding='utf-8')

count = 0
fcount = 0
for line in inf:

    if 'isInside' in line and 'insert' in line:
        count += 1
    else:
        continue


    line = line.replace(' ', '').replace('\n', '').replace('\t', '')


    coor = [3*int(i) for i in line[16:line.find(')')].split(',')]
    coor = [810-coor[i] if i % 2 == 1 else coor[i] for i in range(len(coor))]
    label = line[line.find(')'):][line[line.find(')'):].find('('):][1:-2]
    coor = np.array(coor).reshape([-1, 2])

    cv2.polylines(img, np.int32([coor]), 1, randcolor(), 3)
    if count > 20:
        fcount += 1
        plt.subplot(2, 3, fcount)
        plt.imshow(img)
        img = oriimg.copy()
        count = 0

fcount += 1
plt.subplot(2, 3, fcount)
plt.imshow(img)
img = oriimg.copy()
count = 0

plt.show()
