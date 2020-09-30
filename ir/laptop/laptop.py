import cv2
from extract import extractBoundingBox
from recognize import recognize
from PIL import Image
import time

if __name__ == "__main__":
    cap = cv2.VideoCapture(0)

    models = [
        ("up", cv2.CascadeClassifier('up_cascade.xml')),
        ("down", cv2.CascadeClassifier('down_cascade.xml')),
        ("left", cv2.CascadeClassifier('left_cascade.xml')),
        ("right", cv2.CascadeClassifier('right_cascade.xml')),
        ("w", cv2.CascadeClassifier('w_cascade.xml')),
        ("zero", cv2.CascadeClassifier('zero_cascade.xml'))
    ]
    font=cv2.FONT_HERSHEY_SIMPLEX

    while True:
        ret, img = cap.read() # full image
        bbs = extractBoundingBox(img=img) # potential boundaries
        img_copy = img.copy()
        potential_images = [] # list to hold potential images
        for bb in bbs:
            # for each boundary, extract the image. this is the potential image
            potential_image = img_copy[bb[1]:bb[3], bb[0]:bb[2]]
            potential_image = cv2.resize(potential_image, (24, 24))
            potential_image = cv2.cvtColor(potential_image, cv2.COLOR_BGR2GRAY)
            potential_images.append((potential_image, bb))
            # zx, zy, zw, zh = bb
            # cv2.rectangle(img, (zx, zy), (zx + zw, zy + zh), (255, 255, 255), 2)
        # # check potential images
        # for image in potential_images:
        #     cv2.imshow('img', image)
        #     time.sleep(1)
        #     k = cv2.waitKey(30) & 0xff
        #     if k == 27:
        #         break

        for category, model in models:
            for image, bb in potential_images:
                print("img")

                reg_img = recognize(model=model,
                                    img=image,
                                    category=category)
                print("reg")
                print(reg_img)
                for _ in reg_img:
                    cv2.rectangle(img, (bb[0], bb[1]), (bb[0] + bb[2], bb[1] + bb[3]), (255, 255, 255), 2)
                    cv2.putText(img, category, (bb[0], bb[1] + bb[3] + 40), font, 2, (255, 255, 255), 2)
        cv2.imshow('img', img)
        k = cv2.waitKey(30) & 0xff
        if k == 27:
            break
    cap.release()
    cv2.destroyAllWindows()