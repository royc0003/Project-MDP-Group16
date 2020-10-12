from flask import Flask
from flask_restful import Api, Resource
import cv2
import picamera
import picamera.array
from datetime import datetime
app = Flask(__name__)
api = Api(app)

left_cascade = cv2.CascadeClassifier('left_cascade.xml')
up_cascade = cv2.CascadeClassifier('up_cascade.xml')
right_cascade = cv2.CascadeClassifier('right_cascade.xml')
down_cascade = cv2.CascadeClassifier('down_cascade.xml')

# cam = cv2.VideoCapture(0)


def getImgPred():
    with picamera.PiCamera() as camera:
        camera.resolution = (640, 480)
        with picamera.array.PiRGBArray(camera) as stream:
            camera.start_preview()
            print("before capture" + datetime.now().strftime("%S"))
            camera.capture(stream, format='bgr')
            print("after capture" + datetime.now().strftime("%S"))


            print("before detect img" + datetime.now().strftime("%S"))

            img = stream.array
            print("1 " + datetime.now().strftime("%S"))

            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            print("2 " + datetime.now().strftime("%S"))

            up = up_cascade.detectMultiScale(gray, 1.3, 5)
            down = down_cascade.detectMultiScale(gray, 1.3, 5)
            left = left_cascade.detectMultiScale(gray, 1.3, 5)
            right = right_cascade.detectMultiScale(gray, 1.3, 5)

            up = up_cascade.detectMultiScale(gray, 1.3, 5)
            down = down_cascade.detectMultiScale(gray, 1.3, 5)
            left = left_cascade.detectMultiScale(gray, 1.3, 5)
            right = right_cascade.detectMultiScale(gray, 1.3, 5)

            up = up_cascade.detectMultiScale(gray, 1.3, 5)
            down = down_cascade.detectMultiScale(gray, 1.3, 5)
            left = left_cascade.detectMultiScale(gray, 1.3, 5)
            right = right_cascade.detectMultiScale(gray, 1.3, 5)

            up = up_cascade.detectMultiScale(gray, 1.3, 5)
            down = down_cascade.detectMultiScale(gray, 1.3, 5)
            left = left_cascade.detectMultiScale(gray, 1.3, 5)
            right = right_cascade.detectMultiScale(gray, 1.3, 5)
            
            print("after detect img" + datetime.now().strftime("%S"))

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

            
            print("3 " + datetime.now().strftime("%S"))

    return "none"

class ImgReg(Resource):

    def get(self):
        img_str = getImgPred()
        return {"img": img_str}

api.add_resource(ImgReg, "/ImgReg")

if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True)
