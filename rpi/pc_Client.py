import socket

# rpi's static pi
host = '192.168.16.1'
# port has to the same
port = 5560

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

while True:
    command = input("Enter your command: ")
    
    if command == 'EXIT':
        # Send EXIT request to server
        s.send(str.encode(command))
        break
    elif command == 'KILL':
        # Send KILL request to server
        s.send(str.encode(command))
        break
    
    # send this when it is not any of the above commands
    s.send(str.encode(command))
    # received reply
    reply = s.recv(1024)
    print(reply.decode('utf-8'))

s.close()
