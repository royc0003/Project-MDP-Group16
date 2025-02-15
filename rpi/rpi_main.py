import time
import threading
from rpi_pc import *
#from rpi_bluetooth import *
from rpi_arduino import *


class Main(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        
        self.pc = PC()
#        self.bt = Android()
        self.sr = Arduino()

        self.pc.connect()
 #       self.bt.connect()
        self.sr.connect()
        
        fmessage = '\n-----Main Thread Connected-----'
        print(fmessage)

    def readPCMsg(self):
        while True:
            try:
                pMsg = self.pc.read()
                if not pMsg:
                    break
#                self.pc.write(str(pMsg))
                 #Destination is Tablet
  #              if(pMsg[2] == '0'):
   #                 self.bt.write(pMsg[4:])
                # send to adruino 
                if (pMsg[2] == '1'):
                    self.sr.write(pMsg[4])
                    fmessage = '\nPC > Arduino: ' + str(pMsg[4])
            except Exception as e:
                fmessage = '\nError in PC read: ' + str(e)
                print(fmessage)
                
                # 2|1|F

 #   def readBTMsg(self):
  #      while True:
   #         try:
    #            bMsg = self.bt.read()
     #           if not bMsg:
      #              break
       #         self.bt.write(str(bMsg))
        #        # Destination is PC
         #       if(bMsg[2] == '2'):
          #          self.pc.write(bMsg[4:]) #Note this is to Adrunio
           #     elif(bMsg[2] == '1'):
            #        self.sr.write(bMsg[4:])
           # except Exception as e:
            #    fmessage = '\nError in BT read: ' + str(e)
             #   print(fmessage)

    def readSerialMsg(self):
        while True:
            try:
                sMsg = self.sr.read()
                if not sMsg:
                    break
                self.pc.write(str(sMsg))
            except Exception as e:
                fmessage = '\nError in Serial read: ' + str(e)
                print(fmessage)

    def close(self):
        try:
            self.pc.disconnect()
            fmessage = '\nClosing PC'
            print(fmessage)
          #  self.bt.disconnect()
          #  fmessage = '\nClosing Bluetooth'
          #  print(fmessage)
            self.sr.disconnect()
            fmessage = '\nClosing Serial'
            print(fmessage)
        except Exception as e:
            fmessage = '\nError in Closing at MAIN :' + str(e)
            print(fmessage)
            

if __name__ == "__main__":
    testMain = Main()

    pcReadThread = threading.Thread(target=testMain.readPCMsg, name="PC Read Thread")
 #   blueReadThread = threading.Thread(target=testMain.readBTMsg, name="Bluetooth Read Thread")
    serReadThread = threading.Thread(target=testMain.readSerialMsg, name="Serial Read Thread")
    pcReadThread.daemon = True
  #  blueReadThread.daemon = True
    serReadThread.daemon = True

    pcReadThread.start()
   # blueReadThread.start()
    serReadThread.start()

    fmessage = '-------Begin testing now-------\n'
    print(fmessage)
    
    while True:
        try:
#            print("pcReadThread running now...")
            pcReadThread.join(0.1)
    #        blueReadThread.join(0.1)
 #           print("serReadThread running now...")
            serReadThread.join(0.1)
            
            if not pcReadThread.isAlive():
                break
     #       if not blueReadThread.isAlive():
      #          break
            if not serReadThread.isAlive():
                break
            
        except KeyboardInterrupt:
            print('\nInterrupted! Closing program.')
            testMain.close()
            break
    print('\nInterrupted! Closing program.')
    testMain.close()
      
