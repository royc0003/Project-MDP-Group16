import serial

# receiving input from the Arduino, "Hello World!"
# ser = serial.Serial('/dev/ttyACM0', 9600)
ser = serial.Serial('/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_75833353035351603131-if00', 9600)

# Transmitting output to Arduino, 'S'
testArduino = 'S'
# Converting unicode to bytes; can also convert to something else
testArduinoEncode = testArduino.encode()

while True:
        if(ser.in_waiting > 0):
            # prints the string from the Arduino, "Hello World!"
            line = ser.readline()
            print(line)
            # sending a test string to the Arduino from the Rpi
            ser.write(testArduinoEncode)
