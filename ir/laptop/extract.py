import cv2


def extractBoundingBox(img):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 0)

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
        if 1.3 >= ratio >= 0.7 and area >= 10000:
            bbs.append(bb)

    return bbs
