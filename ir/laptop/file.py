import cv2
import numpy as np


cap=cv2.VideoCapture(0)
font = cv2.FONT_HERSHEY_SIMPLEX

# # Returns 2 parameter, distance in terms of block & direction of img object
# # 1 = 1 block,... 3 = blocks away, l = left, c = center, r = right
# def locateImg(x,y,w,h):
#     area = w*h
#     if (area >= 35000):
#         return ("1","c") #
#     elif (35000 >= area >= 20000):
#         return ("2",boundary(x,y,0))
#     elif (20000 >= area >= 5000):
#         return ("3", boundary(x,y,50))
#
# def boundary (x,y,offset):
#     if (x < (100 + offset)):
#         return "l"
#     elif (x > (450 - offset)):
#         return "r"
#     else:
#         return "c"


while True:
    ret, img = cap.read()
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)




    models = [
        ("up", cv2.CascadeClassifier('../models/up_cascade.xml')),
        ("down", cv2.CascadeClassifier('../models/down_cascade.xml')),
        ("left", cv2.CascadeClassifier('../models/left_cascade.xml')),
        ("right", cv2.CascadeClassifier('../models/right_cascade.xml')),

        ("zero", cv2.CascadeClassifier('../models/zero_cascade_2.xml')), # not very good
        ("nine", cv2.CascadeClassifier('../models/nine_cascade.xml')),
        ("six", cv2.CascadeClassifier('../models/six_cascade.xml')),
        ("eight", cv2.CascadeClassifier('../models/eight_cascade.xml')), # not very good
        ("seven", cv2.CascadeClassifier('../models/seven_cascade.xml')), # not very good

        ("x", cv2.CascadeClassifier('../models/x_cascade.xml')),
        ("z", cv2.CascadeClassifier('../models/z_cascade.xml')),
        ("v2", cv2.CascadeClassifier('../models/v2_cascade.xml')),
        ("y", cv2.CascadeClassifier('../models/y_cascade.xml')),
        ("w2", cv2.CascadeClassifier('../models/w2_cascade.xml')), # not very good

        ("circle", cv2.CascadeClassifier('../models/circle_cascade.xml')),
        ("circle_2", cv2.CascadeClassifier('../models/circle_adpative_threshold_cascade.xml')),

    ]
    # ret, threshold_grey = cv2.threshold(gray, 100, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
    th3 = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 3, 2)
    for category, model in models:
        reg_img = model.detectMultiScale(gray, 1.5, 5)

        for (x, y, w, h) in reg_img:
            # filter out image that are too small
            area = w * h
            if 250000 >= area >= 5000:
                print("x pos is " + str(x))
                cv2.rectangle(gray, (x, y), (x + w, y + h), (255, 255, 255), 2)
                cv2.putText(gray, category + " " + str(area), (x, y + 40), font, 0.5, (255, 255, 255), 2)

    # cv2.imshow('img1', th3)
    cv2.imshow('img', gray)


    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
cap.release()
cv2.destroyAllWindows()
