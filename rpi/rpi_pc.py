import socket
import time
import sys


class PC(object):
    def __init__(self):
        self.tcp_ip = '' # don't change this, leave it blank
        self.port = 5560
        self.conn = None
        self.client = None
        self.addr = None

    def disconnect(self):
        if self.conn:
            self.conn.close()
            #print("Closing server socket")
        if self.client:
            self.client.close()
            #print("Closing client socket")

    def connect(self):
        # Create a TCP/IP socket
        try:
            self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # important to allow reuse of IP
            self.conn.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.conn.bind((self.tcp_ip, self.port))
            # Listen for incoming connections
            self.conn.listen(1)
            print("Listening for incoming connections from PC...")
            self.client, self.addr = self.conn.accept()
            print("Connected! Connection address: ", self.addr)
            
        except Exception as error:
                print('Connection failed: ' + str(error))

    def read(self):
        try:
            message = self.client.recv(2048).strip()
            #return msg_from_pc.decode("utf-8")
            message = message.decode('utf-8')

            if len(message) > 0:
                print('From PC: ' + message)
                return message

            return None

        except Exception as error:
            print('Failed to read from PC: ' + str(error))
            self.close_pc_socket()
            self.connect_pc()

    def write(self, message):
        try:
            #self.client.sendto(str(msg).encode("utf-8"), self.addr)
            print('To PC: ' + message)
            self.client.sendto(str(message).encode('utf-8'), self.addr)
                                        
        except Exception as e:
            print('Failed to write to PC: ' + str(error))
            self.close_pc_socket()
            self.connect_pc()

# If you want this to be the only process, run this.
if __name__ == 'main':
    ser = PC()
    ser.__int__()
    ser.connect()
    print('Ready to transmit and receive!')
    while True:
        try:
            ser.read()
            ser.write('Received input!')
        except KeyboardInterrupt:
            print('PC communication interrupted.')
            ser.disconnect()
            break
