from PIL import Image
from os import listdir
from os.path import isfile, join
import random


RAW_DATA_DIRECTORY = '/Users/ongcj/Dev/mdp/ir/data/processed/zero_resized'
OUTPUT_DIRECTORY = '/Users/ongcj/Dev/mdp/ir/data/training/six/n'
width = 240
COUNT = 20


all_images = [f for f in listdir(RAW_DATA_DIRECTORY) if isfile(join(RAW_DATA_DIRECTORY, f)) and f.endswith('.JPG')]
random.shuffle(all_images)
counter = 0

for image in all_images:
    print(image)
    full_path_to_img = join(RAW_DATA_DIRECTORY, image)
    img = Image.open(full_path_to_img)
    percent = (width / float(img.size[0]))
    resized_img = img.resize((width, int((float(img.size[1] * float(percent)))))).convert('L')
    resized_img.save(join(OUTPUT_DIRECTORY, full_path_to_img.replace(RAW_DATA_DIRECTORY, OUTPUT_DIRECTORY).replace(".JPG", "_RESIZED.JPG")), "JPEG",
                     optimize=True)
    counter = counter + 1
    if counter >= COUNT:
        break
