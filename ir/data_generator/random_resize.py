import random
from PIL import Image
from os import listdir
from os.path import isfile, join


def get_random_width():
    return random.randrange(96, 480)


