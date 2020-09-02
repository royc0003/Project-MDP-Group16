import socket
import time

class PC:
    def __int__(self):
        self.isConnected = False
        self.port = 5560
        self.host = ''
        self.sock = None
        self.client_sock = None
        
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print('Socket has been created.')

        try:
            self.sock.bind((self.host,self.port))
            print('Socket bind complete.')
            self.sock.listen(1)
            
        except socket.error as msg:
            print(msg)

    def connect(self):
        try:
            print('Establishing connection with the PC...')

            if self.client_sock is None:
                self.client_sock, self.address = self.sock.accept()
                print('Connected to: ' + str(self.address))

        except Exception as error:
            print('Connection with PC failed.')

            if self.client_sock is not None:
                self.client_sock.close()
                self.client_sock = None

    def disconnect(self):
        try:
            if self.client_sock is not None:
                self.client_sock.close()
                self.client_sock = None

            if self.socket is not None:
                self.socket.close()
                self.socket = None
                
            print('Successfully closed connection with PC.')
            
        except Exception as error:
            print('Disconnection from the PC is unsuccessful.')

    def read(self):
        try:
            message = self.client_sock.recv(1024)
            message = message.decode('utf-8')

            if len(message) > 0:
                print('From PC: ' + message)
                #print(message)
                return message
            
            return None

        except Exception as error:
            print('Failed to read from PC: ' + str(error))
            raise error

    def write(self, message):
        try:
            print('To PC: ' + message)
            #print(message)
            self.client_sock.send(str.encode(message))

        except Exception as error:
            print('Failed to write to PC: ' + str(error))
            raise(error)

# If you want this to be the main thread, run this.
if __name__ == '__main__':
    ser = PC()
    ser.__int__()
    ser.connect()
    print('Ready to transmit and receive!')
    while True:
        try:
            ser.read()
            ser.write('Recevied Input!')
        except KeyboardInterrupt:
            print('PC Communication interrupted.')
            ser.disconnect()
