from PIL import Image
from os import listdir
from os.path import isfile, join
import random


RAW_DATA_DIRECTORY = '/Users/ongcj/Dev/mdp/ir/data/processed/six_resized'
OUTPUT_DIRECTORY = '/Users/ongcj/Dev/mdp/ir/data/training/six/p'
width = 240


def random_rotation(img):
    # pick a random degree of rotation between 25% on the left and 25% on the right
    random_degree = random.uniform(-15, 15)
    return img.rotate(random_degree, expand=True)


all_images = [f for f in listdir(RAW_DATA_DIRECTORY) if isfile(join(RAW_DATA_DIRECTORY, f)) and f.endswith('.JPG')]
for image in all_images:
    print(image)
    full_path_to_img = join(RAW_DATA_DIRECTORY, image)
    img = Image.open(full_path_to_img)
    percent = (width / float(img.size[0]))
    resized_img = img.resize((width, int((float(img.size[1] * float(percent)))))).convert('L')
    resized_img = random_rotation(resized_img)
    resized_img.save(join(OUTPUT_DIRECTORY, full_path_to_img.replace(RAW_DATA_DIRECTORY, OUTPUT_DIRECTORY).replace(".JPG", "_ROTATED_RESIZED.JPG")), "JPEG",
                     optimize=True)
