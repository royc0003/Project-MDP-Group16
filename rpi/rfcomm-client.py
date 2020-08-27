import bluetooth

bd_addr = "B8:27:EB:E3:A5:37"

port = 4

sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((bd_addr, port))
sock.send(str.encode("Hello from the client!"))

sock.close()
