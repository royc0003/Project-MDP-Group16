from flask import Flask
from flask_restful import Api, Resource
import requests
import cv2
import pickle

app = Flask(__name__)
api = Api(app)
cam = cv2.VideoCapture(0)
cnn_url = 'http://127.0.0.1:5001/predict/'


def takePic():
    ret, img = cam.read()
    img_encoded = cv2.imencode(".jpg", img)[1]
    return img_encoded


class ImgReg(Resource):

    def get(self):
        # this returns a pickle img
        data = takePic()
        headers = {"Content-type": "text/plain"}
        x = requests.post(cnn_url, data=data.tostring(), headers=headers)
        print(x.text)
        return {"result": x.text}


api.add_resource(ImgReg, "/ImgReg")

if __name__ == "__main__":
    app.run(debug=True, port=5000)
