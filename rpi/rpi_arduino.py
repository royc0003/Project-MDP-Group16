import serial 

class Arduino:
    def __init__(self):
        #self.serial_port = '/dev/ttyACM0'
        #self.serial_port = '/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_75833353035351603131-if00'
        self.serial_port = 'COM5'
        self.baud_rate = 9600
        self.connection = None

    def connect(self):
        try:
            print('Establishing connection with Arduino...')

            self.connection = serial.Serial(self.serial_port, self.baud_rate)

            if self.connection is not None:
                print('Successfully connected with Arduino. ')

        except Exception as error:
            print('Connection with Arduino failed: ' + str(error))

    def disconnect(self):
        try:
            if self.connection is not None:
                self.connection.close()
                self.connection = None

                print('Successfully closed connection with Arduino.')

        except Exception as error:
            print('Arduino close connection failed: ' + str(error))
            
    def read(self):
        try:
            self.connection.flush()
            message = self.connection.readline()
            print('From Arduino:')
            print(message)

            if len(message) > 0:
                return message

            return None
       
        except Exception as error:
            print('Arduino read failed: ' + str(error))
            raise error
    
    def write(self, message):
        try:
            #print('To Arduino:')
            #print(message)
            #self.connection.write(str.encode(message))
            self.connection.flush()
            print('To Arduino: ' + message)
            #print(message)
            self.connection.write(str(message).encode('utf-8'))

        except Exception as error:
            print('Arduino write failed: ' + str(error))
            raise error

# If you want this to be the only process, run this.
if __name__ == "__main__":
    ser = Arduino()
    ser.__init__()
    ser.connect()
    while True:
        try:
            ser.read()
            ser.write('S')
            
        except KeyboardInterrupt:
            print('Serial Communication Interrupted.')
            ser.disconnect()
            break
