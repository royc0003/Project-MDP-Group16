# CNN endpoint
This endpoint takes in images and return response containing the predicted result. 

## Running this endpoint 
To run this endpoint, install the required dependency listed in requirements.txt using python 3.6.1.

```bash
python cnn_endpoint.py
```

This endpoint served using flask can be reach at port 5001. And since this is outside of RPI, you need to identify the IP address of the host hosting this endpoint, and replace `cnn_url` in `rpi_image_taking_endpoint.py`

## To-Do: 
- try out colored training data. color seems to be a very strong features. 
