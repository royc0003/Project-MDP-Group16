import time
import threading
import requests
from rpi_pc import *
from rpi_bluetooth import *
from rpi_arduino import *
import json
from collections import deque

# note length is 105 [:105]
#5
# Note when 2|0|STA|DONE PC ---> BT; send to BT done && send all data in q
#
class Main(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.pc = PC()
        self.bt = Android()
        self.sr = Arduino()

        self.pc.connect()
        self.bt.connect()
        self.sr.connect()
        self.delay = 0.2

        # Initialize Queue here
        fmessage = '=====Initializing Q===='
        self.pcQ = deque([])
        self.btQ = deque([])
        self.srQ = deque([])
        self.imgQ = deque([]) #currently not in use

        print(fmessage)

        time.sleep(2)
        fmessage = '\n-----Main Thread Connected-----'
        print(fmessage)



    def readPCMsg(self):
        """
        ReadThread:
        (1) Read a single pc message
        (2) Filter the appropriate messages
        (3) Enqueue to the correct queues
        0 --> Tablet
        1 --> arduino or robot
        5 --> imgReg but still enqueue into PC
        """
        while True:
            try:
                pcMsg = self.pc.read()
                if not pcMsg:
                    break
                # this is to arduino or robot
                if(pcMsg[2] == '1'):
                    self.srQ.append(pcMsg[4])
                # this is to tablet
                elif(pcMsg[2] == '0'):
                    #check for append
                    if(len(pcMsg>105)):
                        self.btQ.append(pcMsg[4:105])
                        # check if is for img reg
                        if(pcMsg[107] == '5'):
                            self.srQ.append(pcMsg[105:])
                    else:
                        self.btQ.append(pcMsg[4:])
                # this is for imgrecognition
                elif(pcMsg[2] == '5'):
                    response = requests.get("http://127.0.0.1:5000/imgreg")
                    img_result = response.json().get("result")
                    c = img_result.get("c").get("category")
                    l = img_result.get("l").get("category")
                    r = img_result.get("r").get("category")
                    if(c is None):
                        c = "-1"
                    if(l is None):
                        l = "-1"
                    if (r is None):
                        r = "-1"
                    pcMsg = c+";"+l+";"+r
                    self.pcQ.append(pcMsg)
            except Exception as e:
                fmessage = '\nError in PC read: ' + str(e)
                print(fmessage)



    def writePCMsg(self):
        """
        PC Write Thread:
        Checks if pc Q has item
        Mainly to send information to PC
        """
        while True:
            if(len(self.pcQ)>0):
                pcMsg = self.pcQ.popleft()
                if(pcMsg):
                    self.pc.write(pcMsg)
                    print("Write to PC: "+ pcMsg)

    def readBTMsg(self):
        """
        ReadBT Thread:
        (1) Read a single BT message
        (2) Filter the appropriate messages
        (3) Enqueue to the correct queues
        0 --> Tablet
        1 --> arduino or robot
        2 --> PC
        5 --> imgReg but still enqueue into PC
        """
        while True:
            try:
                btMsg = self.bt.read()
                if not btMsg:
                    break
                # this is to write to pc
                if(btMsg[2] == "2"):
                    self.pcQ.append(btMsg[4:]+'\n')
                #this is to robot/arduino
                elif(btMsg[2] == "1"):
                    self.srQ.append(btMsg[4:])
            except Exception as e:
                fmessage = '\nError in BT read: ' + str(e)
                print(fmessage)
 
    def writeBTMsg(self):
        """
        BT Write Thread:
        Checks if bt Q has item
        Mainly to send information to Tablet
        """
        while True:
            if(len(self.btQ)>0):
                btMsg= self.btQ.popleft()
                if btMsg:
                    self.bt.write(btMsg)
                    print("WriteBT: Sent to bt: %s" % btMsg)
        

                
    def readSerialMsg(self):
        """
        Read Serial Thread:
        (1) Read a single SR message
        (2) Filter the appropriate messages
        (3) Enqueue to the correct queues
        0 --> Tablet
        1 --> arduino or robot
        2 --> PC
        5 --> imgReg but still enqueue into PC
        """
        while True:
            try:
                srMsg = self.sr.read().decode('utf-8')
                if not srMsg:
                    break
                self.srQ.append(srMsg)
            except Exception as e:
                fmessage = '\nError in Serial read: ' + str(e)
                print(fmessage)
    
    def writeSRMsg(self):
        """
        SR Write Thread:
        Checks if sr Q has item
        Mainly to send information to Robot/ Arduino
        """
        while True:
            if(len(self.srQ)>0):
                srMsg= self.srQ.popleft()
                if srMsg:
                    self.sr.write(srMsg)
                    print("WriteBT: Sent to bt: %s" % srMsg)
    

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


if __name__ == "__main__":
    testMain = Main()

    pcReadThread = threading.Thread(target=testMain.readPCMsg, name="PC Read Thread")
    pcWriteThread = threading.Thread(target=testMain.writePCMsg, name="PC Write Thread")

    blueReadThread = threading.Thread(target=testMain.readBTMsg, name="Bluetooth Read Thread")
    blueWriteThread = threading.Thread(target=testMain.writeBTMsg, name="Bluetooth Write Thread")

    serReadThread = threading.Thread(target=testMain.readSerialMsg, name="Serial Read Thread")
    serWriteThread = threading.Thread(target=testMain.writeSRMsg, name="Serial Write Thread")


    pcReadThread.daemon = True
    pcWriteThread.daemon = True

    blueReadThread.daemon = True
    blueWriteThread.daemon = True

    serReadThread.daemon = True
    serWriteThread.daemon = True

    pcReadThread.start()
    pcWriteThread.start()
    blueReadThread.start()
    blueWriteThread.start()
    serReadThread.start()
    serWriteThread.start()

    fmessage = '-------Begin testing now-------\n'
    print(fmessage)

    while True:
        try:
            pcReadThread.join(0.1)
            pcWriteThread.join(0.1)
            blueReadThread.join(0.1)
            blueWriteThread.join(0.1)
            serReadThread.join(0.1)
            serWriteThread.join(0.1)

            if not pcReadThread.isAlive():
                break
            if not pcWriteThread.isAlive():
                break
            if not blueReadThread.isAlive():
                break
            if not blueWriteThread.isAlive():
                break
            if not serReadThread.isAlive():
                break
            if not serWriteThread.isAlive():
                break

        except KeyboardInterrupt:
            print('\nInterrupted! Closing program.')
            testMain.close()
            break
    print('\nInterrupted! Closing program.')
    testMain.close()
