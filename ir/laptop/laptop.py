import cv2
from recognize import recognize
from PIL import Image
import time

if __name__ == "__main__":
    cap = cv2.VideoCapture(0)

    models = [
        # ("up", cv2.CascadeClassifier('../models/up_cascade.xml')),
        # ("down", cv2.CascadeClassifier('../models/down_cascade.xml')),
        ("left", cv2.CascadeClassifier('../models/left_cascade.xml')),
        ("right", cv2.CascadeClassifier('../models/right_cascade.xml')),

        # ("zero", cv2.CascadeClassifier('../models/zero_cascade.xml')),
        # ("eight", cv2.CascadeClassifier('../models/eight_cascade.xml')),
        ("nine", cv2.CascadeClassifier('../models/nine_cascade.xml')),
        ("six", cv2.CascadeClassifier('../models/nine_cascade.xml'))

        # ("circle", cv2.CascadeClassifier('../models/zero_cascade.xml'))

    ]
    font = cv2.FONT_HERSHEY_SIMPLEX

    while True:
        ret, img = cap.read()  # full image
        # bbs = extractBoundingBox(img=img) # potential boundaries
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray = cv2.GaussianBlur(gray, (5, 5), 0)
        img_copy = img.copy()

        thresh0 = cv2.adaptiveThreshold(
            gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 51, 1)
        contours, _ = cv2.findContours(
            thresh0, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

        # Find bounding box of contours
        bbs = []
        for contour in contours:
            rect = cv2.boundingRect(contour)
            area = rect[2] * rect[3]
            ratio = float(rect[2]) / float(rect[3])
            bb = (rect[0], rect[1], rect[0] + rect[2], rect[1] + rect[3])
            if 1.3 >= ratio >= 0.7 and 250000 >= area >= 50000:
                bbs.append(bb)
                # cv2.rectangle(img, (rect[0], rect[1]), (rect[0] + rect[2], rect[1] + rect[3]), (255, 255, 255), 2)
                # cv2.putText(img, str(area), (rect[0], rect[1] + rect[3] + 40), font, 2, (255, 255, 255), 2)

        potential_images = []  # list to hold potential images
        for bb in bbs:
            # for each boundary, extract the image. this is the potential image
            potential_image = img_copy[bb[1]:bb[3], bb[0]:bb[2]]
            potential_image = cv2.resize(potential_image, (240, 240))
            potential_image = cv2.cvtColor(potential_image, cv2.COLOR_BGR2GRAY)
            potential_images.append((potential_image, bb))

        for category, model in models:
            for image, bb in potential_images:
                reg_img = model.detectMultiScale(image, 1.50, 5)

                for _ in reg_img:
                    cv2.rectangle(img, (bb[0], bb[1]), (bb[2], bb[3]), (255, 255, 255), 2)
                    cv2.putText(img, category, (bb[0], bb[1] + 40), font, 2, (255, 255, 255), 2)

        cv2.imshow('img', img)
        k = cv2.waitKey(30) & 0xff
        if k == 27:
            break

#        print("one iterate")
    cap.release()
    cv2.destroyAllWindows()
