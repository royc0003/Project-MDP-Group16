import cv2
import time
from cnn import CNN
import imutils
import numpy as np
import math


def rotate(origin, point, angle):
    """
    Rotate a point counterclockwise by a given angle around a given origin.

    The angle should be given in radians.
    """
    ox, oy = origin
    px, py = point

    qx = ox + math.cos(angle) * (px - ox) - math.sin(angle) * (py - oy)
    qy = oy + math.sin(angle) * (px - ox) + math.cos(angle) * (py - oy)
    return qx, qy


if __name__ == "__main__":
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 720)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
    font = cv2.FONT_HERSHEY_SIMPLEX
    model = CNN("gray")
    while True:
        ret, img = cap.read()  # full image

        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        img_height, img_width = gray.shape  # original

        original_centre = (img_width // 2, img_height // 2)  # original centre. full shape is 1280, 760

        gray_copy = imutils.rotate_bound(gray, angle=-45)
        copy_height, copy_width = gray_copy.shape
        copy_centre = (copy_width // 2, copy_height // 2)

        # offset from orginal
        offset = ((copy_centre[0] - original_centre[0]), (copy_centre[1] - original_centre[1]))

        thresh0 = cv2.adaptiveThreshold(
            gray_copy, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 51, 0)
        contours, _ = cv2.findContours(
            thresh0, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

        # Find bounding box of contours
        bbs = []
        for contour in contours:
            rect = cv2.boundingRect(contour)  # rect is (x, y, width, height)
            area = rect[2] * rect[3]
            ratio = float(rect[2]) / float(rect[3])
            if 1.2 >= ratio >= 0.8 and 250000 >= area >= 25000:
                # find the centre of the bounding box
                bounding_box_centre = ((rect[0] + (rect[2] // 2)), (rect[1] + (rect[3] // 2)))

                # translate
                translated_centre = (int(bounding_box_centre[0] - offset[0]), int(bounding_box_centre[1] - offset[1]))

                # rotated translated
                rotated_translated_centre = rotate((img_width // 2, img_height // 2), translated_centre, 0.785398)

                rotated_bb = (
                    int(rotated_translated_centre[0] - (rect[2] / 2)),
                    int(rotated_translated_centre[1] - (rect[3] / 2)),
                    int(rotated_translated_centre[0] + (rect[2] / 2)),
                    int(rotated_translated_centre[1] + (rect[3] / 2)))
                if rotated_bb[0] > 0:
                    bbs.append(rotated_bb)


        potential_images = []  # list to hold potential images
        for rotated_bb in bbs:
            potential_image = gray[rotated_bb[1]:rotated_bb[3], rotated_bb[0]:rotated_bb[2]]
            height, width = potential_image.shape
            if height != 0:
                potential_image = cv2.resize(potential_image, (64, 64), interpolation=cv2.INTER_CUBIC)
                potential_image = potential_image / 255
                potential_images.append((potential_image, rotated_bb))
                reg_img = model.predict(potential_image)
                print(reg_img)
                category, prob = reg_img
                if prob > 0.90:
                    x, y, w, h = rotated_bb
                    cv2.rectangle(gray, (rotated_bb[0], rotated_bb[1]), (rotated_bb[2], rotated_bb[3]), (255, 255, 255), 2)
                    cv2.putText(gray, category + "" + str(rotated_bb[2] * rotated_bb[3]), (rotated_bb[0], rotated_bb[1] + 40), font, 2, (255, 255, 255),
                                2)

        cv2.imshow('gray', gray)
        k = cv2.waitKey(30) & 0xff
        if k == 27:
            break

    cap.release()
    cv2.destroyAllWindows()
