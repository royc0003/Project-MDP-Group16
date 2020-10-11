import random
from PIL import Image
from os import listdir
from os.path import isfile, join
import sys

COUNT = 20


def random_rotation(img):
    # pick a random degree of rotation between 25% on the left and 25% on the right
    random_degree = random.uniform(-25, 25)
    return img.rotate(random_degree, expand=True)


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
        resized_img = random_rotation(img)
        resized_img.save(join(OUTPUT_DIRECTORY, full_path_to_img.replace(RAW_DATA_DIRECTORY, OUTPUT_DIRECTORY).replace(".JPG", "_ROTATED_" + str(counter) + ".JPG")), "JPEG",
                         optimize=True)
        counter = counter + 1
        if counter >= COUNT:
            break
