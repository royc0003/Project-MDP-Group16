# README
`grey_and_resize.py` is to process raw images captured from devices.

`random_sample.py` randomly sample images from the given directory and output to the specified directory

`random_rotate.py` randomly picks images from the given directory and generate a new images with a random rotation between -25 to 25 degree.

`random_resize` resize all of the images randomly between of width 96 to 480 with proportional ratio to height in the given directory and remove old images. 

## To generate positive images
Change the directory in `generate_p_training_set.bash`.
```bash
#!/bin/bash

# first random sample 100 from processed data
python random_sample.py <NUMBER TO SAMPLE> <PATH_TO_PROCESSED_DATA> <PATH_TO_TARGET>
# out of the random 100 samples, random rotate 30 images
python random_rotate.py <NUMBER TO ROTATE> <PATH_TO_SAMPLED_DATA> <PATH_TO_TARGET>
# lastly, random resize everything
python random_resize.py <NUMBER TO RESIZE> <PATH_TO_SAMPLED_DATA> <PATH_TO_TARGET>
```
- NUMBER TO SAMPLE/ROTATE/RESIZE : This value should be lower than number of images in the directory.
- PATH_TO_PROCESSED_DATA : A directory that stores processed images. These images should be the output of `grey_and_resize.py`.
- PATH_TO_SAMPLED_DATA : A directory that store the sampled data.
- PATH_TO_TARGET : Target directory to store the result.
Run `bash generate_p_training_set.bash`  