import subprocess
from bluetooth import *

class Android(object):
    def __init__(self):
        subprocess.call(["pkill","blueman-applet"])
        subprocess.call(["sudo","hciconfig","hci0","piscan"])
        self.server_socket = None
        self.client_socket = None

    def disconnect(self):
        if self.client_socket:
            self.client_socket.close()
            print("Closing client socket")
        if self.server_socket:
            self.server_socket.close()
            print("Closing server socket")

    def connect(self):
        # Creating the server socket and bind to port           
        btport = 8
        try:
            self.server_socket = BluetoothSocket( RFCOMM )
            self.server_socket.bind(("", btport))
            # Listen for requests
            self.server_socket.listen(1)
            self.port = self.server_socket.getsockname()[1]
            uuid = '92c1090e-e85b-11ea-adc1-0242ac120002'

            advertise_service( self.server_socket, 'MDPGrp16BT',
                                service_id = uuid,
                                service_classes = [ uuid, SERIAL_PORT_CLASS ],
                                profiles = [ SERIAL_PORT_PROFILE ], )
            print('Waiting for BT connection on RFCOMM channel %d' % self.port)
            # Accept requests
            self.client_socket, client_address = self.server_socket.accept()
            print('Accepted connection from ', client_address)

        except Exception as error:
            print('Connection Error - Bluetooth: ' + str(error))
            

    def read(self):
        try:
            message = self.client_socket.recv(2048).decode('utf-8').strip()
            print('From Android: ' + message)
            
            if not message:
                return None

            return message

        except BluetoothError:
            print('\nBluetooth Read Error. Connection lost')
            # Re-establish connection
            

    def write(self, message):
        try:
            print('To Android: ' + message)
            self.client_socket.send(str(message).encode('utf-8'))

        except BluetoothError:
            print('\nBluetooth Write Error. Connection lost')
            # Re-establish connection
            

# If you want this to be the only process, run this.
if __name__ == "__main__":
    ser = Android()
    ser.__init__()
    ser.connect()
    while True:
        try:
            print('In while loop...')
            ser.read()
            ser.write('From Rpi!')
        except KeyboardInterrupt:
            print('Android communication interrupted.')
            ser.disconnect()
            break
        
