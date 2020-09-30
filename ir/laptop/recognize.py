import cv2


def recognize(model, img, category):
    reg_img = model.detectMultiScale(img, 1.03, 5)
    # for(x, y, w, h) in reg_img:
    #     cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)
    #     cv2.putText(img, category, (x, y + h + 40), font, 2, (255, 0, 0), 2)
    #
    # cv2.imshow('img', reg_img)
    return reg_img


