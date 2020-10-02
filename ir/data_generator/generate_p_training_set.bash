#!/bin/bash

# first random sample 100 from processed data
python random_sample.py 50 /Users/ongcj/Dev/mdp/ir/data/processed/nine_resized /Users/ongcj/Dev/mdp/ir/data/training/nine/p
# out of the random 100 samples, random rotate 30 images
python random_rotate.py 50 /Users/ongcj/Dev/mdp/ir/data/training/nine/p /Users/ongcj/Dev/mdp/ir/data/training/nine/p
# lastly, random resize everything
python random_resize.py 100 /Users/ongcj/Dev/mdp/ir/data/training/nine/p /Users/ongcj/Dev/mdp/ir/data/training/nine/p

