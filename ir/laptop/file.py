import cv2
import numpy as np
left_cascade = cv2.CascadeClassifier('left_cascade.xml')
up_cascade = cv2.CascadeClassifier('up_cascade.xml')
right_cascade = cv2.CascadeClassifier('right_cascade.xml')
down_cascade = cv2.CascadeClassifier('down_cascade.xml')
zero_cascade = cv2.CascadeClassifier('zero_cascade.xml')
w_cascade = cv2.CascadeClassifier('w_cascade.xml')

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

        # ("zero", cv2.CascadeClassifier('../models/zero_cascade.xml')),
        ("eight", cv2.CascadeClassifier('../models/cascade-3.xml')),
        ("nine", cv2.CascadeClassifier('../models/nine_cascade.xml')),
        ("six", cv2.CascadeClassifier('../models/six_cascade.xml'))

        # ("circle", cv2.CascadeClassifier('../models/zero_cascade.xml'))

    ]

    for category, model in models:
        reg_img = model.detectMultiScale(gray, 1.30, 5)

        for (x, y, w, h) in reg_img:
            # filter out image that are too small
            area = w * h
            if 250000 >= area >= 50000:
                cv2.rectangle(gray, (x, y), (x + w, y + h), (255, 255, 255), 2)
                cv2.putText(gray, category + str(area), (x, y + 40), font, 2, (255, 255, 255), 2)




    cv2.imshow('img',gray)
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
cap.release()
cv2.destroyAllWindows()
