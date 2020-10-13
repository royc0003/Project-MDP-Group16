# Image Recognition Docker on RPI
### Environment setting (If you need the display to be shown on RPI)
- Check if the environment variable has been set.

    `ECHO $DISPLAY`

- If not set, 
    `export DISPLAY=:0.0`

- Allow docker clients to connect to host,

    `xhost +`



### Python-OpenCV-Flask

#### Build the image

`sudo docker build python-opencv/. -t python-opencv`

#### Run the docker image

1. Check the path for IR repo (You need to ensure you are on the correct branch.). This is the `hostDirectory` which need to be replaced in the bash below.
2. Run the bash.
```bash
sudo docker run -ti \
    --device=/dev/vcsm \
    --device=/dev/vchiq \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
    -v /home/pi/Desktop/repo/ir:/home/pi/Desktop/repo/ir \
    -p 5000:5000 \
    python-opencv:latest

```

#### Run your script
Running the above bash will bring you into the container. 

The directory of the image recognition code is set to `/home/pi/Desktop/repo/ir`.

```bash
cd /home/pi/Desktop/repo/ir/rpi
python checklist_test_rpi.py
```

exit




