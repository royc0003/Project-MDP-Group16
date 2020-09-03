import time
import threading
from rpi_arduino import *
from rpi_pc import *
from rpi_bluetooth import *


class Main(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.sr = Arduino()
        self.pc = PC()
        self.bt = Android()
        self.sr.connect()
        self.pc.connect()
        self.bt.connect()
        fmessage = 'Sleep for a while'
        print(fmessage)
        time.sleep(0.5)
        fmessage = '\nMain Thread Connected'
        print(fmessage)

    def readPCMsg(self):
        while True:
            try:
                pMsg = self.pc.read()
                # Destination is Tablet
                if (pMsg[1] == '0'):
                    self.bt.write(pMsg[2:])
                # Destination is Arduino
                elif (pMsg[1] == '1'):
                    self.sr.write(pMsg[2:])
                    fmessage = '\nPC > Arduino: ' + str(pMsg[2:])
            except Exception as e:
                fmessage = '\nError in PC read: ' + str(e)
                print(fmessage)

    def readBTMsg(self):
        while True:
            try:
                bMsg = self.bt.read()
                # Destination is PC
                if (bMsg[1] == '2'):
                    self.pc.write(bMsg[2:])
                # Destination is Arduino
                elif(bMsg[1] == '1'):
                    self.sr.write(bMsg[2:])
            except Exception as e:
                fmessage = '\nError in BT read: ' + str(e)
                print(fmessage)

    def readSerialMsg(self):
        while True:
            try:
                sMsg = self.sr.read()
                self.pc.write(str(sMsg))
            except Exception as e:
                fmessage = '\nError in Serial read: ' + str(e)
                print(fmessage)

    def threadStart(self):
        pcReadThread = threading.Thread(target=self.readPCMsg, name="PC Read Thread")
        pcWriteThread = threading.Thread(target=self.pc.write, args=("",), name="PC Write Thread")

        fmessage = '\nPC Read/Write Thread'
        print(fmessage)

        blueReadThread = threading.Thread(target=self.readBTMsg, name="Bluetooth Read Thread")
        blueWriteThread = threading.Thread(target=self.bt.write, args=("",), name="Bluetooth Write Thread")

        fmessage = '\nBluetooth Read/Write Thread'
        print(fmessage)

        serReadThread = threading.Thread(target=self.readSerialMsg, name="Serial Read Thread")
        serWriteThread = threading.Thread(target=self.sr.write, args=("",), name="Serial Write Thread")

        fmessage = '\nSerial Read/Write Thread'
        print(fmessage)

        pcReadThread.daemon = True
        pcWriteThread.daemon = True
        blueReadThread.daemon = True
        blueWriteThread.daemon = True
        serReadThread.daemon = True
        serWriteThread.daemon = True
        fmessage = '\nAll Threads Daemon = True'
        print(fmessage)

        pcReadThread.start()
        pcWriteThread.start()
        blueReadThread.start()
        blueWriteThread.start()
        serReadThread.start()
        serWriteThread.start()

        pcReadThread.join()
        pcWriteThread.join()
        blueReadThread.join()
        blueWriteThread.join()
        serReadThread.join()
        serWriteThread.join()

        fmessage = '\nAll Read Write Thread Started\n'
        print(fmessage)

    def close(self):
        try:
            self.pc.disconnect()
            fmessage = '\nClosing PC'
            print(fmessage)
            self.bt.disconnect()
            fmessage = '\nClosing Bluetooth'
            print(fmessage)
            self.sr.disconnect()
            fmessage = '\nClosing Serial'
            print(fmessage)
        except Exception as e:
            fmessage = '\nError in Closing at MAIN :' + str(e)
            print(fmessage)

    def sleepTimer(self):
        while True:
            time.sleep(0.5)


if __name__ == "__main__":
    testMain = Main()
    try:
        testMain.threadStart()
        testMain.sleepTimer()
    except KeyboardInterrupt:
        print('\nMain Interrupted')
        testMain.close()
