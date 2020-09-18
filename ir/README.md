```bash
docker run -ti --device=/dev/vcsm \
    --device=/dev/vchiq \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
    -v hostDirectoryPath:containerDirectory \
    sgtwilko/rpi-raspbian-opencv:latest

```