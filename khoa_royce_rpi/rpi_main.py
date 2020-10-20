import time
import threading
import requests
from rpi_pc import *
from rpi_bluetooth import *
from rpi_arduino import *
import json

class Main(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

        self.pc = PC()
        self.bt = Android()
        self.sr = Arduino()

        self.pc.connect()
        self.bt.connect()
        self.sr.connect()
        time.sleep(2)
        fmessage = '\n-----Main Thread Connected-----'
        print(fmessage)

    def writePCMsg(self, msg_to_pc):
        self.pc.write(msg_to_pc)
        print("WritePC: Sent to PC: %s" % msg_to_pc)

    def readPCMsg(self):
        while True:
            try:
                pMsg = self.pc.read()
                if not pMsg:
                    break
                #                self.pc.write(str(pMsg))
                # Destination is Tablet
                #              if(pMsg[2] == '0'):
                #                 self.bt.write(pMsg[4:])
                # send to adruino 
                if (pMsg[2] == '1'):
                    self.writeSRMsg(pMsg[4])
                    fmessage = '\nPC > Arduino: ' + str(pMsg[4])
                # Destination is Tablet
                elif (pMsg[2] == '0'):
                    self.writeBTMsg(pMsg[4:])
            except Exception as e:
                fmessage = '\nError in PC read: ' + str(e)
                print(fmessage)

                # 2|1|F

    def writeBTMsg(self, msg_to_bt):
        self.bt.write(msg_to_bt)
        print("WriteBT: Sent to bt: %s" % msg_to_bt)

    def readBTMsg(self):
        while True:
            try:
                bMsg = self.bt.read()
                if not bMsg:
                    break
                # Destination is PC
                if (bMsg[2] == '2'):
                    self.writePCMsg(bMsg[4:] + '\n')  # Note this is to Adrunio
                elif (bMsg[2] == '1'):
                    self.writeSRMsg(bMsg[4:])
            except Exception as e:
                fmessage = '\nError in BT read: ' + str(e)
                print(fmessage)

    def writeSRMsg(self, msg_to_sr):
        self.sr.write(msg_to_sr)
        print("WriteSR: Sent to SR: %s" % msg_to_sr)
<<<<<<< HEAD
    
    def readImgMsg(self):
        while True:
            try: 
                pMsg = self.pc.read()
                if not pMsg:
                    break
                # note this is for taking image
                if (pMsg[2] == '5'):
                    response = requests.get("http://127.0.0.1:5000/imgreg")
                    img_result = response.json().get("result")
                    c = img_result.get("c")
                    l = img_result.get("l")
                    r = imge_result.get("r")
                    if(c is None):
                        c = "-1"
                    if(l is None):
                        l = "-1"
                    if (r is None):
                        r = "-1"
                    pMsg = c+";"+l+";"+r
                    self.writePCMsg(pMsg+'\n')
            except Exception as e:
                fmessage = '\nError in Img read: ' + str(e)
                print(fmessage)
                
=======

>>>>>>> 750e8fea2f60cf9168518b17fd3931f896a3cf8e
    def readSerialMsg(self):
        while True:
            try:
                sMsg = self.sr.read().decode('utf-8')
<<<<<<< HEAD
                
=======
                # length = len(sMsg)-2
                # sMsg = sMsg[:length]
>>>>>>> 750e8fea2f60cf9168518b17fd3931f896a3cf8e
                if not sMsg:
                    break
                
#                print(sMsg)
 #               sMsg = sMsg.replace("\n","")
                #print(sMsg)
            
                #response = requests.get("http://127.0.0.1:5000/imgreg")
                #img_result = response.json().get('result')
                #img_result= json.loads(img_result)
                #if(len(img_result)>=1):
                    #img_result = img_result[0]
                    #img = img_result['image']
                    #pos = img_result['position']
                #else:
  #              img = "-1"
   #             pos = "-1"
                    
                #sMsg += "7_" + img + ";8_" + pos + '\n'
                
    #            sMsg2 = "7_" + img + ";8_" + pos + '\n'
     #           print("sMsg = " + sMsg)
      #          print("sMsg2 = " + sMsg2)
       #         sMsg3 = sMsg + sMsg2
        #        print("sMsg3 = " + sMsg3)
                
         #       print("Printing after append: " + sMsg)
                self.writePCMsg(sMsg)
            except Exception as e:
                fmessage = '\nError in Serial read: ' + str(e)
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


if __name__ == "__main__":
    testMain = Main()

    pcReadThread = threading.Thread(target=testMain.readPCMsg, name="PC Read Thread")
    pcWriteThread = threading.Thread(target=testMain.writePCMsg, args=("",), name="PC Write Thread")

    blueReadThread = threading.Thread(target=testMain.readBTMsg, name="Bluetooth Read Thread")
    blueWriteThread = threading.Thread(target=testMain.writeBTMsg, args=("",), name="Bluetooth Write Thread")

    serReadThread = threading.Thread(target=testMain.readSerialMsg, name="Serial Read Thread")
    serWriteThread = threading.Thread(target=testMain.writeSRMsg, args=("",), name="Serial Write Thread")

<<<<<<< HEAD
    imgReadThread = threading.Thread(target=testMain.readImgMsg, name="Img Read Thread")
#readImgMsg

=======
>>>>>>> 750e8fea2f60cf9168518b17fd3931f896a3cf8e
    pcReadThread.daemon = True
    pcWriteThread.daemon = True

    blueReadThread.daemon = True
    blueWriteThread.daemon = True

    serReadThread.daemon = True
    serWriteThread.daemon = True

<<<<<<< HEAD
    imgReadThread.daemon = True

=======
>>>>>>> 750e8fea2f60cf9168518b17fd3931f896a3cf8e
    pcReadThread.start()
    pcWriteThread.start()
    blueReadThread.start()
    blueWriteThread.start()
    serReadThread.start()
    serWriteThread.start()
    imgReadThread.start()

    fmessage = '-------Begin testing now-------\n'
    print(fmessage)

    while True:
        try:
<<<<<<< HEAD
            #print("pcReadThread running now...")
            pcReadThread.join(0.1)
            #print("btReadThread running now...")
            blueReadThread.join(0.1)
            #print("serReadThread running now...")
            serReadThread.join(0.1)
            imgReadThread.join(0.1)
=======
            # print("pcReadThread running now...")
            # pcReadThread.join(0.1)
            # print("btReadThread running now...")
            # blueReadThread.join(0.1)
            # print("serReadThread running now...")
            # serReadThread.join(0.1)
>>>>>>> 750e8fea2f60cf9168518b17fd3931f896a3cf8e
            time.sleep(1)
            if not pcReadThread.isAlive():
                break
            if not blueReadThread.isAlive():
                break
            if not serReadThread.isAlive():
                break

        except KeyboardInterrupt:
            print('\nInterrupted! Closing program.')
            testMain.close()
            break
    print('\nInterrupted! Closing program.')
    testMain.close()
