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
        self.msgQ = deque([])
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
                self.msgQ.append(pcMsg)
            except Exception as e:
                fmessage = '\nError in PC read: ' + str(e)
                print(fmessage)




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
                self.msgQ.append(btMsg)
            except Exception as e:
                fmessage = '\nError in BT read: ' + str(e)
                print(fmessage)
 


                
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
                self.msgQ.append(srMsg)
            except Exception as e:
                fmessage = '\nError in Serial read: ' + str(e)
                print(fmessage)
    
    def writeMsg(self):
        """
        0 --> Tablet
        1 --> arduino
        2 --> PC
        5 --> imgReg
        """
        while True:
            if(len(self.msgQ)>0):
                sourceMsg= self.msgQ.popleft()
                sourceNo = None
                if(sourceMsg[0] == '2'): #pc
                    sourceNo = 2
                elif(sourceMsg[0] == '0'): # tablet
                    sourceNo = 0
                else:
                    sourceNo = 1
                
                if(sourceNo == 2): # pc
                    if (sourceMsg[2] == '1'):
                        self.sr.write(sourceMsg[4])
                    elif (sourceMsg[2] == '0'):
                        self.bt.write(sourceMsg[4:])
                    elif (sourceMsg[2] == '5'): # take image
                        response = requests.get("http://127.0.0.1:5000/imgreg")
                        img_result = response.json().get("result")
                        c = img_result.get("c")
                        l = img_result.get("l")
                        r = img_result.get("r")
                        if(c is None):
                            c = "-1"
                        else:
                            c = c.get('category')
                        if(l is None):
                            l = "-1"
                        else:
                            l = l.get('category')
                        if (r is None):
                            r = "-1"
                        else:
                            r = r.get('category')
                        pcMsg = c+";"+l+";"+r
                        self.pc.write(pcMsg)
                elif(sourceNo == 0):
                    if (sourceMsg[2] == '2'):
                        self.pc.write(sourceMsg[4:] + '\n')  # Note this is to Adrunio
                    elif (sourceMsg[2] == '1'):
                        self.sr.write(sourceMsg[4:])
                elif(sourceNo == 1): # arduino
                    self.pc.write(sourceMsg)


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
    blueReadThread = threading.Thread(target=testMain.readBTMsg, name="Bluetooth Read Thread")
    serReadThread = threading.Thread(target=testMain.readSerialMsg, name="Serial Read Thread")
    singleWriteThread =threading.Thread(target=testMain.writeMsg, name="Single Write Thread")

    pcReadThread.daemon = True
    blueReadThread.daemon = True
    serReadThread.daemon = True
    singleWriteThread.daemon = True

    pcReadThread.start()
    blueReadThread.start()
    serReadThread.start()
    singleWriteThread.start()

    fmessage = '-------Begin testing now-------\n'
    print(fmessage)

    while True:
        try:
            pcReadThread.join(0.1)
            blueReadThread.join(0.1)
            serReadThread.join(0.1)
            singleWriteThread.join(0.1)

            if not pcReadThread.isAlive():
                break
            if not blueReadThread.isAlive():
                break  
            if not serReadThread.isAlive():
                break
            if not singleWriteThread.isAlive():
                break
        
                

        except KeyboardInterrupt:
            print('\nInterrupted! Closing program.')
            testMain.close()
            break
    print('\nInterrupted! Closing program.')
    testMain.close()
