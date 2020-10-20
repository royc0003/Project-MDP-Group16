import time
import threading
from rpi_pc import *
from rpi_bluetooth import *
from rpi_arduino import *


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

    def readSerialMsg(self):
        while True:
            try:
                sMsg = self.sr.read().decode('utf-8')
                # length = len(sMsg)-2
                # sMsg = sMsg[:length]
                if not sMsg:
                    break
                # index some value;
                # if some value >= 3 LRLeft
                # -1 = x <= 2 || -1 <= y <= 4
                # variable_image = call API end point to take image
                # variable_value = variable_imeage['img']
                # self.writePCMsg (str(sMsg)+variable_value)
                self.writePCMsg(str(sMsg))
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
            # print("pcReadThread running now...")
            # pcReadThread.join(0.1)
            # print("btReadThread running now...")
            # blueReadThread.join(0.1)
            # print("serReadThread running now...")
            # serReadThread.join(0.1)
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
