import time
import requests
from multiprocessing import Process, Queue
from rpi_pc import *
from rpi_bluetooth import *
from rpi_arduino import *


def writePCMsg(msg_to_pc):
    pc.write(msg_to_pc)
    print("WritePC: Sent to PC: %s" % msg_to_pc)

def readPCMsg():
    while True:
        try:
            pMsg = pc.read()
            if not pMsg:
                break
            fmessage = '\nPC > PC: ' + str(pMsg[4])

            #self.pc.write(str(pMsg))
            # Destination is Tablet
            #if(pMsg[2] == '0'):
                #self.bt.write(pMsg[4:])

            # Destination is Arduino
            if (pMsg[2] == '1'):
                writeSRMsg(pMsg[4])
                fmessage = '\nPC > Arduino: ' + str(pMsg[4])
            # Destination is Tablet 
            elif(pMsg[2] == '0'):
                writeBTMsg(pMsg[4:])
        except Exception as e:
            fmessage = '\nError in PC read: ' + str(e)
            print(fmessage)

def writeBTMsg(msg_to_bt):
    bt.write(msg_to_bt)
    print("WriteBT: Sent to bt: %s" % msg_to_bt)

def readBTMsg():
    while True:
        try:
            bMsg = bt.read()
            if not bMsg:
                break
            # Destination is PC
            if(bMsg[2] == '2'):
                writePCMsg(bMsg[4:]+'\n') # Note this is to Adruino
            elif(bMsg[2] == '1'):
                writeSRMsg(bMsg[4:])
        except Exception as e:
            fmessage = '\nError in BT read: ' + str(e)
            print(fmessage)

def writeSRMsg(msg_to_sr):
    sr.write(msg_to_sr)
    print("WriteSR: Sent to SR: %s" % msg_to_sr)

def readSerialMsg():
    while True:
        try:
            sMsg = sr.read().decode('utf-8')
            if not sMsg:
                break

            dataSplit = sMsg.split(';', 5)
            testString = dataSplit[4]
            if(testString[2] == '3' or testString[2] == '4'):
                msgQueue.put(sMsg)
                continue
            writePCMsg(str(sMsg))
        except Exception as e:
            fmessage = '\nError in Serial read: ' + str(e)
            print(fmessage)

def image_rec():
    try:
        while True:
            message = msgQueue.get()

            if message == None:
                continue

            try: 
                response = requests.get("http://127.0.0.1:5000/ImgReg")
                sMsg = message + ';7_' + response.json().get('img')
                writePCMsg(str(sMsg))

            except Exception as e:
                fmessage = '\nError in getting Image Rec response: ' + str(e)
                
    except Exception as e:
        fmessage = '\nError in Image Rec Process: ' + str(e)
        print(fmessage)

def close():
    try:
        pc.disconnect()
        fmessage = '\nClosing PC'
        print(fmessage)
        bt.disconnect()
        fmessage = '\nClosing Bluetooth'
        print(fmessage)
        sr.disconnect()
        fmessage = '\nClosing Serial'
        print(fmessage)
    except Exception as e:
        fmessage = '\nError in Closing at MAIN :' + str(e)
        print(fmessage)


if __name__ == "__main__":
    try:
        
        pc = PC()
        bt = Android()
        sr = Arduino()

        pc.connect()
        bt.connect()
        sr.connect()

        time.sleep(2)
        fmessage = '\n-------Begin testing now-------\n'
        print(fmessage)

        msgQueue = Queue()
        PCListener = Process(target=readPCMsg)
        androidListener = Process(target=readBTMsg)
        arduinoListener = Process(target=readSerialMsg)
        imageListener = Process(target=image_rec) #4th process for image rec

        PCListener.start()
        androidListener.start()
        arduinoListener.start()
        imageListener.start()

    except KeyboardInterrupt:
        print('\nInterrupted! Closing program.')
        close()
