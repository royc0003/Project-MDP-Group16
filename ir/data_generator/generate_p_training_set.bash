#!/bin/bash

# first random sample 100 from processed data
python random_sample.py 50 /Users/ongcj/Dev/mdp/ir/data/processed/six_resized /Users/ongcj/Dev/mdp/ir/data/training/six/p
# out of the random 100 samples, random rotate 30 images
python random_rotate.py 50 /Users/ongcj/Dev/mdp/ir/data/training/six/p /Users/ongcj/Dev/mdp/ir/data/training/six/p
# lastly, random resize everything
python random_resize.py 100 /Users/ongcj/Dev/mdp/ir/data/training/six/p /Users/ongcj/Dev/mdp/ir/data/training/six/p

