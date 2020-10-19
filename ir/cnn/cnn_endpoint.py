from flask import Flask, request, jsonify, make_response
from flask_restful import Resource, Api
import pickle
import numpy as np
from model.cnn import CNN
import cv2

cnn = CNN("gray")
app = Flask(__name__)
api = Api(app)


class Predict(Resource):
    def post(self):
        data = request.data
        img = cv2.imdecode(np.fromstring(data, dtype=np.uint8), cv2.IMREAD_COLOR)
        results = cnn.raw_predict(img)

        return make_response(jsonify(results), 201)


api.add_resource(Predict, '/predict')
if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True, port=5000)
