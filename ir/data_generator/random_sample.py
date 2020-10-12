import random
from PIL import Image
from os import listdir
from os.path import isfile, join
import sys

RAW_DATA_DIRECTORY = '/Users/ongcj/Dev/mdp/ir/data/processed/zero_resized'
OUTPUT_DIRECTORY = '/Users/ongcj/Dev/mdp/ir/data/training/six/n'
COUNT = 100


if __name__ == "__main__":
    COUNT = int(sys.argv[1])
    RAW_DATA_DIRECTORY = sys.argv[2] #'/Users/ongcj/Dev/mdp/ir/data/processed/zero_resized'
    OUTPUT_DIRECTORY = sys.argv[3] # '/Users/ongcj/Dev/mdp/ir/data/training/six/n'
    all_images = [f for f in listdir(RAW_DATA_DIRECTORY) if isfile(join(RAW_DATA_DIRECTORY, f)) and f.endswith('.JPG')]
    random.shuffle(all_images)

    counter = 0
    for image in all_images:
        print(image)
        full_path_to_img = join(RAW_DATA_DIRECTORY, image)
        img = Image.open(full_path_to_img)
        img.save(join(OUTPUT_DIRECTORY, full_path_to_img.replace(RAW_DATA_DIRECTORY, OUTPUT_DIRECTORY)), "JPEG", optimize=True)
        counter = counter + 1
        if counter >= COUNT:
            break
