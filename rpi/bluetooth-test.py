#!/usr/bin
from bluetooth import *
server_sock = BluetoothSocket(RFCOMM)
server_sock.bind(("",5))
server_sock.listen(1)
port = server_sock.getsockname()[1]
uuid = "92c1090e-e85b-11ea-adc1-0242ac120002"
advertise_service( server_sock, "MDPGrp16BT",
service_id = uuid,
service_classes = [ uuid, SERIAL_PORT_CLASS ],
profiles = [ SERIAL_PORT_PROFILE ],
# protocols = [ OBEX_UUID ]
)
print("Waiting for connection on RFCOMM channel %d" % port)
client_sock, client_info = server_sock.accept()
print("Accepted connection from ", client_info)
try:
	while True:
		print ("In while loop...")
		data = client_sock.recv(1024)
		if len(data) == 0: break
		print("Received [%s]" % data)
		client_sock.send(data + "i am pi!".encode('ascii'))
except IOError:
	pass
print("disconnected")
client_sock.close()
server_sock.close()
print("all done")