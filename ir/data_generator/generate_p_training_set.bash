#!/bin/bash

# first random sample 100 from processed data
python random_sample.py 50 /Users/ongcj/Dev/mdp/ir/data/processed/seven_resized /Users/ongcj/Dev/mdp/ir/data/training/seven/p
# out of the random 100 samples, random rotate 30 images
python random_rotate.py 50 /Users/ongcj/Dev/mdp/ir/data/training/seven/p /Users/ongcj/Dev/mdp/ir/data/training/seven/p
# random resize everything
python random_resize.py 100 /Users/ongcj/Dev/mdp/ir/data/training/seven/p /Users/ongcj/Dev/mdp/ir/data/training/seven/p
# generate left right top bottom perspective
python generate_perspective.py /Users/ongcj/Dev/mdp/ir/data/processed/seven_resized/IMG_1353_RESIZED.JPG /Users/ongcj/Dev/mdp/ir/data/training/seven/p

