import requests
import pickle
import cv2
from PIL import Image
import numpy as np

BASE = "http://127.0.0.1:5000/"

response = requests.get(BASE + "ImgReg")

print(response.text)
with open("data.pkl", 'rb') as pickle_file:
    content = pickle.load(pickle_file)

# print(content)
print(content)
pil_img_f = Image.fromarray(content.astype(np.uint8))
print(pil_img_f)

cv2.imshow("test", pil_img_f)
# im = np.array(Image.open(content))


# cv2.imwrite("testing_"+str(1)+".jpg", data)
# img = io.StringIO(data.content)
# data = response.json()
#
# print(data)