# Image Recognition

## Build the docker image

`sudo docker build . -t ir-endpoint`

## Run the docker image

```bash
docker run -ti --device=/dev/vcsm \
    --device=/dev/vchiq \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \ 
    -v /home/pi/Desktop/repo/ir/rpi:/home/pi/Desktop/repo/ir/rpi \
    -p 5000:5000 \
    ir-endpoint

```

## How to run RPI 

The directory of the image recognition code is set to `/home/pi/Desktop/repo/ir/rpi`, which is also the mdp git repo. Currently only 2 branches, ir and master, contain the image recognition code. You will need to checkout either of the branch. 

When in the container, run 
```bash
cd /home/pi/Desktop/repo/ir/rpi
python file.py
```



