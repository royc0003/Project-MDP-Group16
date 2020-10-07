import cv2
import time
import math
import imutils
from picamera.array import PiRGBArray
from picamera import PiCamera

font = cv2.FONT_HERSHEY_SIMPLEX

model_normal = [
        ("up", cv2.CascadeClassifier('up_cascade.xml')),
        ("down", cv2.CascadeClassifier('down_cascade.xml')),
        ("left", cv2.CascadeClassifier('left_cascade.xml')),
        ("right", cv2.CascadeClassifier('right_cascade.xml')),

        ("x", cv2.CascadeClassifier('x_cascade.xml')),
        ("z", cv2.CascadeClassifier('z_cascade.xml')),
        ("v", cv2.CascadeClassifier('v_cascade.xml')),
        ("y", cv2.CascadeClassifier('y_cascade.xml')),

        ("circle", cv2.CascadeClassifier('circle_adpative_threshold_cascade.xml')),
    ]

model_rotated = [
    ("zero", cv2.CascadeClassifier('zero_cascade_2.xml')),  # not very good
    ("nine", cv2.CascadeClassifier('nine_cascade.xml')),
    ("six", cv2.CascadeClassifier('six_cascade.xml')),
    ("eight", cv2.CascadeClassifier('eight_cascade.xml')),  # not very good
    ("seven", cv2.CascadeClassifier('seven_cascade.xml')),  # not very good

    ("w", cv2.CascadeClassifier('w_cascade.xml')),  # not very good
]

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

cap=PiCamera()
cap.resolution = (640,480)
cap.framerate=40
rawCapture=PiRGBArray(cap,size=(640,480))
time.sleep(0.1)


for frame in cap.capture_continuous(rawCapture,format="bgr",use_video_port=True):

    img=frame.array
    print(type(img))
    rawCapture.truncate(0)

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img_height, img_width = gray.shape  # original

    original_centre = (img_width // 2, img_height // 2)  # original centre. full shape is 1280, 760

    gray_copy2 = gray  # for countours for non-rotating

    gray_copy = imutils.rotate_bound(gray, angle=-45)  # for countours for rotating
    copy_height, copy_width = gray_copy.shape
    copy_centre = (copy_width // 2, copy_height // 2)

    # offset from orginal
    offset = ((copy_centre[0] - original_centre[0]), (copy_centre[1] - original_centre[1]))

    ## for countours that are rotated
    thresh0 = cv2.adaptiveThreshold(
        gray_copy, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 51, 0)
    contours, _ = cv2.findContours(
        thresh0, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

    ## for countours that are not rotated
    thresh1 = cv2.adaptiveThreshold(
        gray_copy2, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 51, 0)
    contours_nom, _ = cv2.findContours(
        thresh1, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

    # Find bounding box of contours for rotated
    bbs_rotated = []
    for contour in contours:
        rect = cv2.boundingRect(contour)  # rect is (x, y, width, height)
        area = rect[2] * rect[3]
        ratio = float(rect[2]) / float(rect[3])
        if 1.2 >= ratio >= 0.8 and 250000 >= area >= 5000:
            # find the centre of the boudning box '
            bounding_box_centre = ((rect[0] + (rect[2] // 2)), (rect[1] + (rect[3] // 2)))

            # translate
            translated_centre = (int(bounding_box_centre[0] - offset[0]), int(bounding_box_centre[1] - offset[1]))

            # rotated translated
            rotated_translated_centre = rotate((img_width // 2, img_height // 2), translated_centre, 0.785398)

            # use the trainoated centre to find the bottom left op right, offset by 55 to make crop image bigger

            rotated_bb = (
                int(rotated_translated_centre[0] - (rect[2] / 2) - 55),
                int(rotated_translated_centre[1] - (rect[3] / 2) - 55),
                int(rotated_translated_centre[0] + (rect[2] / 2) + 55),
                int(rotated_translated_centre[1] + (rect[3] / 2) + 55))

            if (rotated_bb[0] > 0 and rotated_bb[1] > 0):
                bbs_rotated.append(rotated_bb)

    # find contours that are not rotated
    bbs_normal = []
    for contour in contours_nom:
        rect = cv2.boundingRect(contour)  # rect is (x, y, width, height)
        area = rect[2] * rect[3]
        ratio = float(rect[2]) / float(rect[3])
        if 1.2 >= ratio >= 0.8 and 250000 >= area >= 5000:
            # print(rect)
            x = rect[0]
            y = rect[1]
            w = rect[2]
            h = rect[3]
            bb = x - 50, y - 50, x + w + 50, y + h + 50
            # print("bb = " + str(bb))
            if ((bb[0] > 0 and bb[1] > 0)):
                bbs_normal.append(bb)

    potential_images_normal = []  # list to hold potential images
    potential_images_rotated = []

    # print("bbs = " + str(bbs))
    for rotated_bb in bbs_rotated:

        # for each boundary, extract the image. this is the potential image
        potential_image = gray[rotated_bb[1]:rotated_bb[3], rotated_bb[0]:rotated_bb[2]]

        height, width = potential_image.shape
        if ((height or width) != 0):
            potential_images_rotated.append((potential_image, rotated_bb))

    for rotated_bb in bbs_normal:
        # for each boundary, extract the image. this is the potential image
        potential_image = gray[rotated_bb[1]:rotated_bb[3], rotated_bb[0]:rotated_bb[2]]

        # print(potential_image.shape)
        height, width = potential_image.shape
        if ((height or width) != 0):
            potential_images_normal.append((potential_image, rotated_bb))

    font = cv2.FONT_HERSHEY_SIMPLEX
    for category, model in model_normal:
        for image, bb in potential_images_normal:
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
                # cv2.putText(gray, category + "" + str(bb[2] * bb[3]), (bb[0], bb[1] + 40), font, 2, (255, 255, 255), 2)
                cv2.putText(gray, category, (bb[0], bb[1] + 40), font, 2, (255, 255, 255), 2)

    for category, model in model_rotated:
        for image, bb in potential_images_rotated:
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
                # cv2.putText(gray, category + "" + str(bb[2] * bb[3]), (bb[0], bb[1] + 40), font, 2, (255, 255, 255), 2)
                cv2.putText(gray, category, (bb[0], bb[1] + 40), font, 2, (255, 255, 255), 2)

    cv2.imshow('gray', gray)
  #  ret, img = cap.read()
    cv2.imshow('img',img)
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break

cap.release()
cv2.destroyAllWindows()
