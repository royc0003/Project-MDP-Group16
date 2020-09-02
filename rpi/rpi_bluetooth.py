import bluetooth
import time

class Android:
    def __int__(self):
        self.server_sock = None
        self.client_sock = None

        self.server_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        self.server_sock.bind(('',4))
        self.server_sock.listen(1)
        port = self.server_sock.getsockname()[1]

        uuid = '92c1090e-e85b-11ea-adc1-0242ac120002'

        bluetooth.advertise_service(
            self.server_sock, 'MDPGrp16BT', service_id = uuid,
            service_classes = [ uuid, bluetooth.SERIAL_PORT_CLASS ],
            profiles = [ bluetooth.SERIAL_PORT_PROFILE ]
        )
        print('Waiting for connection on RFCOMM channel %d' % port)

    def connect(self):
        try:
            print('Establishing connection with the Android...')

            if self.client_sock is None:
                self.client_sock, address = self.server_sock.accept()
                print('Connected to: ' + str(address))

        except Exception as error:
            print('Connection with Android failed.' + str(error))
            if self.client_sock is not None:
                self.client_sock.close()
                self.client_sock = None
                
    def disconnect(self):
        try:
            if self.client_sock is not None:
                self.client_sock.close()
                self.client_sock = None
                self.server_sock.close()
                self.server_sock = None
                
            print('Android disconnected successfully.')

        except Exception as error:
            print('Disconnection with Android failed.' + str(error))
            
    def read(self):
        try:
            message = self.client_sock.recv(1024)
            message = message.decode('utf-8')
            
            print('From Android: ' + message)

            if len(message) > 0:
                return message

            return None

        except Exception as error:
            print('Failed to read from Android: ' + str(error))
            raise error

    def write(self, message):
        try:
            print('To Android: ' + message)
            self.client_sock.send(str.encode(message))
            
        except Exception as error:
            print('Failed to write to Android: ' + str(error))
            raise error

if __name__ == "__main__":
    ser = Android()
    ser.__int__()
    ser.connect()
    while True:
        try:
            print('In while loop...')
            ser.read()
            ser.write('From Rpi!')
        except KeyboardInterrupt:
            print('Android communication interrupted.')
            ser.disconnect()
    ser.close()
