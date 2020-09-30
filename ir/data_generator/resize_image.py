from PIL import Image
from os import listdir
from os.path import isfile, join

RAW_DATA_DIRECTORY = '/Users/ongcj/Desktop/environment'
width = 240

all_images = [f for f in listdir(RAW_DATA_DIRECTORY) if isfile(join(RAW_DATA_DIRECTORY, f)) and f.endswith('.jpg')]
for image in all_images:
    print(image)
    full_path_to_img = join(RAW_DATA_DIRECTORY, image)
    img = Image.open(full_path_to_img)
    percent = (width/float(img.size[0]))
    resized_img = img.resize(   (width, int((float(img.size[1] * float(percent)))))   ).rotate(-90, expand=True).convert('L')
    resized_img.save(join(RAW_DATA_DIRECTORY, full_path_to_img.replace(".JPG", "_RESIZED.JPG")), "JPEG", optimize=True)
