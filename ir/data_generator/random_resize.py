import random
from PIL import Image
from os import listdir
from os.path import isfile, join
import sys
import os

COUNT = 20


def get_random_width():
    return random.randrange(96, 480)


if __name__ == "__main__":
    COUNT = int(sys.argv[1])
    RAW_DATA_DIRECTORY = sys.argv[2] #'/Users/ongcj/Dev/mdp/ir/data/training/six/p'
    OUTPUT_DIRECTORY = sys.argv[3] #'/Users/ongcj/Dev/mdp/ir/data/training/six/p'
    all_images = [f for f in listdir(RAW_DATA_DIRECTORY) if isfile(join(RAW_DATA_DIRECTORY, f)) and f.endswith('.JPG')]
    random.shuffle(all_images)
    counter = 0
    for image in all_images:
        print(image)
        full_path_to_img = join(RAW_DATA_DIRECTORY, image)
        img = Image.open(full_path_to_img)
        width = get_random_width()
        print("randomed width :" + str(width))
        percent = (width / float(img.size[0]))
        resized_img = img.resize((width, int((float(img.size[1] * float(percent)))))).convert('L')
        resized_img.save(join(OUTPUT_DIRECTORY, full_path_to_img.replace(RAW_DATA_DIRECTORY, OUTPUT_DIRECTORY).replace(".JPG", "_RANDOM_RESIZE"+ str(counter) +".JPG")), "JPEG",
                         optimize=True)
        os.remove(full_path_to_img)
        counter = counter + 1
        if counter >= COUNT:
            break
