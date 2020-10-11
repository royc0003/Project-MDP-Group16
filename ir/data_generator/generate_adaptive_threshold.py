from PIL import Image
from os import listdir
from os.path import isfile, join
import random
import cv2


DATA_DIRECTORY = '/Users/ongcj/Dev/mdp/ir/data/training/circle/n'

all_images = [f for f in listdir(DATA_DIRECTORY) if isfile(join(DATA_DIRECTORY, f)) and f.endswith('.jpg')]
random.shuffle(all_images)

for image in all_images:
    print(image)
    full_path_to_img = join(DATA_DIRECTORY, image)
    img = cv2.imread(full_path_to_img)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    th3 = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)

    cv2.imwrite(full_path_to_img.replace(".JPG", "_THRESHOLD.JPG"), th3)

