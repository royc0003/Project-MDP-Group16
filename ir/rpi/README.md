# RPI endpoint
This is the endpoint that is set up in RPI. This endpoint handle a GET request, which will then take a picture using the RPI camera, post the taken image to the CNN end point that is outside of the RPI. 

Follow the instruction in ./Docker and creates a container in the RPI. Run this `rpi_image_taking_endpoint.py` script inside the container to start the flask server. 
```bash
python rpi_image_taking_endpoint.py
```

The flask server can be reached at http://127.0.0.1:5000/ImgReg