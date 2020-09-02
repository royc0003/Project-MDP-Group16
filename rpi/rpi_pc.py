import socket

class PC():
    def __int__(self):
        self.isConnected = False
        self.port = 5560
        self.host = ''
        #self.address = ''
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
                print('Connected to: ' + str(address[1]))

        except Exception as error:
            print('Connection with PC failed.')

            if self.client_sock is not None:
                self.client_sock.close()
                self.client_sock = None


if __name__ == '__main__':
    ser = PC()
    ser.__int__()
    ser.connect()
