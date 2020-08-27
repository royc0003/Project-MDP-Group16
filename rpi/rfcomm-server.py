import bluetooth

server_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

port = 4
host = ''
server_sock.bind((host,port))
server_sock.listen(1)
print("Server is listening on port 4")

client_socket, address = server_sock.accept()
print("Accepted connection from ", address)

data = client_sock.recv(1024)
print(data.decode('utf-8'))

client_sock.close()
server_sock.close()
