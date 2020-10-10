from flask import Flask
from flask_restful import Api, Resource
import cv2
import codecs, json
import numpy as np
import pickle

app = Flask(__name__)
api = Api(app)

# left_cascade = cv2.CascadeClassifier('../models/left_cascade.xml')
# up_cascade = cv2.CascadeClassifier('../models/up_cascade.xml')
# right_cascade = cv2.CascadeClassifier('../models/right_cascade.xml')
# down_cascade = cv2.CascadeClassifier('../models/down_cascade.xml')

cam = cv2.VideoCapture(0)


def takePic():
    ret, img = cam.read()
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    file = open('data.pkl', 'wb')

    pickle.dump(gray, file)
    file.close()

    return file


class ImgReg(Resource):

    def get(self):
        data = takePic()
        print(data)
        return {"data": str(data)}


api.add_resource(ImgReg, "/ImgReg")

if __name__ == "__main__":
    app.run(debug=True)
