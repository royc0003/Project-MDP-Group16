import cv2
from model.cnn import CNN
import imutils
import math


if __name__ == "__main__":
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 720)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)
    font = cv2.FONT_HERSHEY_SIMPLEX
    model = CNN("gray")
    while True:
        ret, img = cap.read()  # full image
        gray, results = model.raw_predict(img)
        print(results)
        cv2.imshow('gray', gray)
        k = cv2.waitKey(30) & 0xff
        if k == 27:
            break

    cap.release()
    cv2.destroyAllWindows()
