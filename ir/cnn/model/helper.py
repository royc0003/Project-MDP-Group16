import cv2
import imutils
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


def find_contour_at_45(img):
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
        if 1.3 >= ratio >= 0.7 and 12000 >= area >= 8000:
            # find the centre of the bounding box
            # print(area)
            bounding_box_centre = ((rect[0] + (rect[2] // 2)), (rect[1] + (rect[3] // 2)))

            # translate
            translated_centre = (int(bounding_box_centre[0] - offset[0]), int(bounding_box_centre[1] - offset[1]))

            # rotated translated
            rotated_translated_centre = rotate((img_width // 2, img_height // 2), translated_centre, 0.785398)

            print(rotated_translated_centre)
            rotated_bb = (
                int(rotated_translated_centre[0] - (rect[2] / 2)),
                int(rotated_translated_centre[1] - (rect[3] / 2)),
                int(rotated_translated_centre[0] + (rect[2] / 2)),
                int(rotated_translated_centre[1] + (rect[3] / 2)))
            if rotated_bb[0] > 0:
                # approximate l, r and c
                # mid point of detected img is at
                mid_point = rotated_translated_centre[0]
                # spilt the image into 3 parts / units
                one_unit = img_width // 3
                if mid_point < one_unit:
                    bbs.append((rotated_bb, 'l'))
                elif one_unit <= mid_point < (one_unit * 2):
                    bbs.append((rotated_bb, 'c'))
                else:
                    bbs.append((rotated_bb, 'r'))

    return bbs


def find_contour(img):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    thresh0 = cv2.adaptiveThreshold(
        gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 51, 0)
    contours, _ = cv2.findContours(
        thresh0, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    img_height, img_width = gray.shape  # original
    one_unit = img_width // 3
    bbs = []
    for contour in contours:
        rect = cv2.boundingRect(contour)  # rect is (x, y, width, height)
        area = rect[2] * rect[3]
        ratio = float(rect[2]) / float(rect[3])
        if 1.3 >= ratio >= 0.7 and 30000 >= area >= 8000:
            mid_point = rect[0] + rect[2] // 2
            if mid_point < one_unit:
                position = 'l'
            elif one_unit <= mid_point < (one_unit * 2):
                position = 'c'
            else:
                position = 'r'
            bbs.append(
                ((rect[0], rect[1], rect[0] + rect[2], rect[1] + rect[3]),
                 position)
            )

    bbs_at_45 = find_contour_at_45(img)

    # append both non 45 and 45
    bbs = bbs + bbs_at_45

    return bbs
