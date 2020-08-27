#import bluetooth

#bd_addr = "B8:27:EB:12:0D:6F"

#port = 4

#sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
#sock.connect((bd_addr, port))

#sock.send("hello!!")

#sock.close()

import sys
from bluetooth import *

uuid = "0x0003-0000-1000-8000-00805F9B34FB"
service_matches = bluetooth.find_service( uuid = uuid )
testMsg = "Hello from client!"

if len(service_matches) == 0:
    print ("couldn't find the FooBar service")
    sys.exit(0)

first_match = service_matches[0]
port = first_match["4"]
name = first_match["raspberrypi"]
host = first_match["192.168.1.20"]

print ("connecting to \"%s\" on %s" % (name, host))

sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((host, port))
sock.send(str.encode(testMsg))
sock.close()
