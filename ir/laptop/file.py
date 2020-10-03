import cv2
import numpy as np


cap=cv2.VideoCapture(0)
font = cv2.FONT_HERSHEY_SIMPLEX

while True:
    ret, img = cap.read()
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)




    models = [
        ("up", cv2.CascadeClassifier('../models/up_cascade.xml')),
        ("down", cv2.CascadeClassifier('../models/down_cascade.xml')),
        ("left", cv2.CascadeClassifier('../models/left_cascade.xml')),
        ("right", cv2.CascadeClassifier('../models/right_cascade.xml')),

        ("zero", cv2.CascadeClassifier('../models/zero_cascade.xml')), # not very good
        ("nine", cv2.CascadeClassifier('../models/nine_cascade.xml')),
        ("six", cv2.CascadeClassifier('../models/six_cascade.xml')),
        ("eight", cv2.CascadeClassifier('../models/eight_cascade.xml')), # not very good
        ("seven", cv2.CascadeClassifier('../models/seven_cascade.xml')), # not very good

        ("x", cv2.CascadeClassifier('../models/x_cascade.xml')),
        ("z", cv2.CascadeClassifier('../models/z_cascade.xml')),
        ("v", cv2.CascadeClassifier('../models/v_cascade.xml')),
        ("y", cv2.CascadeClassifier('../models/y_cascade.xml')),
        ("w", cv2.CascadeClassifier('../models/w_cascade.xml')), # not very good

        ("circle", cv2.CascadeClassifier('../models/circle_cascade.xml')),

    ]
    # ret, threshold_grey = cv2.threshold(gray, 100, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
    th2 = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 11, 2)
    th3 = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
    for category, model in models:
        reg_img = model.detectMultiScale(gray, 1.30, 5)

        for (x, y, w, h) in reg_img:
            # filter out image that are too small
            area = w * h
            if 250000 >= area >= 50000:
                cv2.rectangle(gray, (x, y), (x + w, y + h), (255, 255, 255), 2)
                cv2.putText(gray, category + " " + str(area), (x, y + 40), font, 2, (255, 255, 255), 2)

    cv2.imshow('img1', th3)
    cv2.imshow('img2', th2)
    cv2.imshow('img', gray)


    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
cap.release()
cv2.destroyAllWindows()
