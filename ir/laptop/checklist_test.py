import cv2
import time
# from cnn import CNN
import imutils
import numpy as np
import math


def cart2pol(x, y):
    theta = np.arctan2(y, x)
    rho = np.hypot(x, y)
    return theta, rho


def pol2cart(theta, rho):
    x = rho * np.cos(theta)
    y = rho * np.sin(theta)
    return x, y


def rotate_contour(cnt, angle):
    M = cv2.moments(cnt)
    cx = int(M['m10'] / M['m00'])
    cy = int(M['m01'] / M['m00'])

    cnt_norm = cnt - [cx, cy]

    coordinates = cnt_norm[:, 0, :]
    xs, ys = coordinates[:, 0], coordinates[:, 1]
    thetas, rhos = cart2pol(xs, ys)

    thetas = np.rad2deg(thetas)
    thetas = (thetas + angle) % 360
    thetas = np.deg2rad(thetas)

    xs, ys = pol2cart(thetas, rhos)

    cnt_norm[:, 0, 0] = xs
    cnt_norm[:, 0, 1] = ys

    cnt_rotated = cnt_norm + [cx, cy]
    cnt_rotated = cnt_rotated.astype(np.int32)

    return cnt_rotated


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
    # model = CNN("gray")
    model2 = [
        ("up", cv2.CascadeClassifier('../models/up_cascade.xml')),
        ("down", cv2.CascadeClassifier('../models/down_cascade.xml')),
        ("left", cv2.CascadeClassifier('../models/left_cascade.xml')),
        ("right", cv2.CascadeClassifier('../models/right_cascade.xml')),

        ("zero", cv2.CascadeClassifier('../models/zero_cascade_2.xml')),  # not very good
        ("nine", cv2.CascadeClassifier('../models/nine_cascade.xml')),
        ("six", cv2.CascadeClassifier('../models/six_cascade.xml')),
        ("eight", cv2.CascadeClassifier('../models/eight_cascade.xml')),  # not very good
        ("seven", cv2.CascadeClassifier('../models/seven_cascade.xml')),  # not very good

        ("x", cv2.CascadeClassifier('../models/x_cascade.xml')),
        ("z", cv2.CascadeClassifier('../models/z_cascade.xml')),
        ("v2", cv2.CascadeClassifier('../models/v2_cascade.xml')),
        ("y", cv2.CascadeClassifier('../models/y_cascade.xml')),
        ("w2", cv2.CascadeClassifier('../models/w2_cascade.xml')),  # not very good

        # ("circle", cv2.CascadeClassifier('../models/circle_cascade.xml')),
        ("circle_2", cv2.CascadeClassifier('../models/circle_adpative_threshold_cascade.xml')),

    ]

    while True:
        ret, img = cap.read()  # full image

        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        img_height, img_width = gray.shape  # original

        original_centre = (img_width // 2, img_height // 2)  # original centre. full shape is 1280, 760

        gray_copy = imutils.rotate_bound(gray, angle=-45)
        copy_height, copy_width = gray_copy.shape
        copy_centre = (copy_width // 2, copy_height // 2)
        # print("orginal centre")
        # print((original_centre))
        # print("copy centre")
        # print(copy_centre)

        # offset from orginal
        offset = ((copy_centre[0] - original_centre[0]), (copy_centre[1] - original_centre[1]))
        # print("offset")
        # print(offset)


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
            if 1.2 >= ratio >= 0.8 and 250000 >= area >= 5000:
                # find the centre of the boudning box '
                bounding_box_centre = ((rect[0] + (rect[2] // 2)), (rect[1] + (rect[3] // 2)))
                # print("bouning_box centre")
                # print(bounding_box_centre)

                # translate
                translated_centre = (int(bounding_box_centre[0] - offset[0]), int(bounding_box_centre[1] - offset[1]))
                # print("translated_centre")
                # print(translated_centre)

                # rotated translated
                rotated_translated_centre = rotate((img_width // 2, img_height // 2), translated_centre, 0.785398)
                # print("rotated_translated_centre")
                # print(rotated_translated_centre)

                # use the trainoated centre to find the bottom left op right

                rotated_bb = (
                int(rotated_translated_centre[0] - (rect[2] / 2) - 55), int(rotated_translated_centre[1] - (rect[3] / 2) - 55 ),
                int(rotated_translated_centre[0] + (rect[2] / 2) + 55), int(rotated_translated_centre[1] + (rect[3] / 2) + 55))


                if rotated_bb[0] > 0:
                    bbs.append(rotated_bb)

                # print(rotated_bb)
                # bottom_left = (bb[0], bb[1])
                # new_bottom_left = translate_rotate_point(bottom_left, (img_width/2, img_height/2), 45)
                # top_right = (bb[2], bb[3])
                # new_top_right = translate_rotate_point(top_right, (img_width/2, img_height/2), 45)

                # gray_copy = cv2.rectangle(gray_copy,
                #                           (rect[0], rect[1]),
                #                           (rect[0] + rect[2], rect[1] + rect[3]),
                #                           (255, 255, 0), 2)
                # gray = cv2.rectangle(gray,
                #                      (rotated_bb[0], rotated_bb[1]),
                #                      (rotated_bb[2],  rotated_bb[3]),
                #                      (255, 255, 0), 2)

                # cv2.putText(gray, str(area), (rect[0], rect[1] + rect[3] + 40), font, 2, (255, 255, 255), 2)


        potential_images = []  # list to hold potential images
        for rotated_bb in bbs:
            # print("slice--------------")
            # print(rotated_bb)
            # for each boundary, extract the image. this is the potential image
            # print(gray.shape)
            potential_image = gray[rotated_bb[1]:rotated_bb[3], rotated_bb[0]:rotated_bb[2]]

            # print(potential_image.shape)
            height, width = potential_image.shape
            if height != 0:
                # print("here")
                # potential_image = cv2.resize(potential_image, (64, 64), interpolation=cv2.INTER_CUBIC)
                # potential_image = potential_image / 255
                potential_images.append((potential_image, rotated_bb))

        # for image, bb in potential_images:

        #     reg_img = model.predict(image)
        #     bestResults = [None, 0, None]
        #     print(reg_img)
        #     category, prob = reg_img
        #     if prob > 0.80:
        #         x, y, w, h = bb
        #         cv2.rectangle(gray, (bb[0], bb[1]), (bb[2], bb[3]), (255, 255, 255), 2)
        #         cv2.putText(gray, category + "" + str(bb[2] * bb[3]), (bb[0], bb[1] + 40), font, 2, (255, 255, 255), 2)

        font = cv2.FONT_HERSHEY_SIMPLEX
        for category, model in model2:
            for image, bb in potential_images:
                # print("image = " + str(image.shape))
                cv2.imshow('image', image)
                # image = np.array(image, dtype='uint8')
                reg_img = model.detectMultiScale(image, 1.5, 5)
                # print(reg_img)
                    # filter out image that are too small
                    # print("x pos is " + str(x))
                if (len(reg_img) != 0):
                    x, y, w, h = bb
                    cv2.rectangle(gray, (bb[0], bb[1]), (bb[2], bb[3]), (255, 255, 255), 2)
                    cv2.putText(gray, category + "" + str(bb[2] * bb[3]), (bb[0], bb[1] + 40), font, 2, (255, 255, 255), 2)



        # gray = imutils.rotate(gray, angle=-45)
        cv2.imshow('gray', gray)
        # cv2.imshow('gray_copy', gray_copy)

        k = cv2.waitKey(30) & 0xff
        if k == 27:
            break

    #        print("one iterate")
    cap.release()
    cv2.destroyAllWindows()
