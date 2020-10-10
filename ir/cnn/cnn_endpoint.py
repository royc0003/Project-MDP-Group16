from flask import FLask, request

import pickle
from .model.cnn import CNN

cnn = CNN("gray")
app = Flask(__name__)


@app.route('/', methods=['POST'])
def receiveImage():
    content = request.data
    img = pickle.loads(content)  # get serialized data
      # return a no content response
