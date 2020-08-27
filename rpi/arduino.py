import serial

# receiving input from the Arduino, "Hello World!"
ser = serial.Serial('/dev/ttyACM0', 9600)

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
