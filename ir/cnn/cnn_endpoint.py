from flask import Flask, request, jsonify
from flask_restful import Resource, Api
import pickle
from model.cnn import CNN

cnn = CNN("gray")
app = Flask(__name__)
api = Api(app)


class Predict(Resource):
    def post(self):
        content = request.data
        img = pickle.loads(content)  # get serialized data
        gray, results = cnn.raw_predict(img)
        json_results = []
        for result in results:
            json_result = {
                'image': result[0],
                'position': results[1]
            }
            json_results.append(json_result)
        return jsonify(json_results), 201


api.add_resource(Predict, '/predict/')
if __name__ == '__main__':
    app.run(debug=True)
