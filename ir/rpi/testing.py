import time
import sys
import math
from multiprocessing import Queue
from picamera.array import PiRGBArray
import picamera
from time import sleep
import numpy as np
import io


with picamera.PiCamera() as camera:
   camera.start_preview()
   time.sleep(2)
   for filename in camera.capture_continuous('img{counter:03d}.jpg'):
       print('Captured %s' % filename)
       time.sleep(10)
    
    
    

   


