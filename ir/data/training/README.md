- Added random rotation in processing to improve the accuracy.
- Add more negative sample.



Some of the images are harder to train due to similarities. we add more images to neg set.
eight :
30 env
40 zero
40 nine
40 six
40 circle

# six
## p
 
## n
30 env

*upright*
10 zero 
10 nine
10 eight

*random rotate from upright, this add to upright*
30 

*random_resize(upright + random rotate from upright). this overwrite the above*
60 

