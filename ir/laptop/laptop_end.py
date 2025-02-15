from flask import Flask
from flask_restful import Api, Resource
import cv2

app = Flask(__name__)
api = Api(app)

left_cascade = cv2.CascadeClassifier('../models/left_cascade.xml')
up_cascade = cv2.CascadeClassifier('../models/up_cascade.xml')
right_cascade = cv2.CascadeClassifier('../models/right_cascade.xml')
down_cascade = cv2.CascadeClassifier('../models/down_cascade.xml')

cam = cv2.VideoCapture(0)


def getImgPred():

    ret, img = cam.read()
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    up = up_cascade.detectMultiScale(gray, 1.3, 5)
    down = down_cascade.detectMultiScale(gray, 1.3, 5)
    left = left_cascade.detectMultiScale(gray, 1.3, 5)
    right = right_cascade.detectMultiScale(gray, 1.3, 5)
    if (len(up) != 0):
        print("up is detected")
        return "up"
    if (len(down) != 0):
        print("down is detected")
        return "down"
    if (len(right) != 0):
        print("right is detected")
        return "right"
    if (len(left) != 0):
        print("left is detected")
        return "left"

    return "none"

class ImgReg(Resource):

    def get(self):
        img_str = getImgPred()
        return {"img": img_str}

api.add_resource(ImgReg, "/ImgReg")

if __name__ == "__main__":
    app.run(debug=True)