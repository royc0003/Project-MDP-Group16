from flask import Flask
from flask_restful import Api, Resource
import requests
import cv2
import picamera
import picamera.array
import json


cnn_url = 'http://127.0.0.1:5000/predict'
app = Flask(__name__)
api = Api(app)


def takePic():
    with picamera.PiCamera() as camera:
        camera.resolution = (640, 480)
        with picamera.array.PiRGBArray(camera) as stream:
            camera.start_preview()
            camera.capture(stream, format='bgr')
            img = stream.array
            img_encoded = cv2.imencode(".jpg", img)[1]
            return img_encoded


class ImgReg(Resource):

    def get(self):
        # this returns a pickle img
        data = takePic()
        headers = {"Content-type": "text/plain"}
        x = requests.post(cnn_url, data=data.tostring(), headers=headers)
        return {"result": json.loads(x.text)}


api.add_resource(ImgReg, "/imgreg")

if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True, port=5000)
