#!/usr/bin/env python3

import serial
import time
import binascii
import mysql
from mysql import connector

import dbconn

class connector:
    def __init__(self, port = "/dev/ttyUSB0", bound = 115200, timeout = 1):
        try:
            self.se = serial.Serial(port, bound,
                timeout = timeout,
                parity = serial.PARITY_NONE,
                stopbits = serial.STOPBITS_ONE,
                bytesize = serial.EIGHTBITS)
        except:
            print("Fail to open device " + port)
            exit()

        self.se.write("AT\r\n".encode())
        time.sleep(0.2)
        self.se.write("AT\r\n".encode())
        time.sleep(0.2)
        self.se.flushInput()

        self.se.write("AT+CPIN?\r\n".encode())
        time.sleep(0.2)
        if self.se.readall().decode("utf8").find("READY") < 0:
            print("SIM Init Fail, system exit")
            exit()

        self.se.write("AT+CMGF=1\r\n".encode())
        time.sleep(0.2)
        if self.se.readall().decode("utf8").find("OK") < 0:
            print("SIM Init Fail, system exit")
            exit()

        self.se.write("AT+CSCS=\"UCS2\"\r\n".encode())
        self.se.write("AT+GSMBUSY=1\r\n".encode())
        time.sleep(0.2)
        if self.se.readall().decode("utf8").find("ERROR") > -1:
            print("SIM Init Fail, system exit")
            exit()


    def send(self, phone = "10086", message = "Nothing"):
        self.se.flushInput()
        ucsphone = binascii.hexlify(str(phone).encode("utf-16be"))
        ucsmessage = binascii.hexlify(str(message).encode("utf-16be"))
        # command = "AT+CMGS=\"".encode() + ucsphone + "\"\r\n".encode() + ucsmessage + b"\x1A"
        self.se.write("AT+CMGS=\"".encode() + ucsphone + "\"".encode())
        time.sleep(0.1)
        self.se.write("\r\n".encode())
        time.sleep(0.1)
        self.se.write(ucsmessage)
        time.sleep(0.1)
        self.se.write(b'\x1a')
        time.sleep(4)
        ret = self.se.readall()
        if ret.decode("utf8").find('OK') > 0:
            return True
        print("Send Fail, Trace:[  " + ret + "  ]")
        return False

    def receive(self, delete = True):
        self.se.flushInput()
        self.se.write("AT+CMGL=\"ALL\"\r\n".encode())
        time.sleep(0.1)
        cmgl = self.se.readall()

        msgs = []
        for eachMsg in cmgl.split(b"\r\n\r\n"):
            if (eachMsg.find(b"CMTI") < 0) & (eachMsg.find(b'AT') < 0):
                new_msg = []
                t = eachMsg.split(b'"')
                try:
                    # phone from
                    new_msg.append(binascii.unhexlify(t[3]).decode("utf-16be"))
                    # time
                    new_msg.append(str(time.mktime(time.strptime(t[7].decode()[:-3], "%y/%m/%d,%H:%M:%S")))[:-2])
                    # message
                    new_msg.append(binascii.unhexlify(t[8][2:]).decode("utf-16be"))
                    # print("New Message: [ " + str(new_msg) + " ]")
                    msgs.append(new_msg)
                except IndexError as e:
                    continue

        if delete:
            self.se.write(b"AT+CMGDA=\"DEL ALL\"\r\n")
            time.sleep(0.1)
            self.se.flushInput()
        return msgs

    def __del__(self):
        self.se.close()


if __name__ == "__main__":
    try:
        db_conn = mysql.connector.connect(host = 'localhost', user = 'root', password = 'uniquestudio')
        db_cursor = db_conn.cursor()
    except Exception as e:
        print(e)
        print("Fail to connect to server")

    s = connector()

    print("Init Finished")

    while True:
        receive_queue = s.receive()
        send_queue = dbconn.query_send()

        if receive_queue:
            #Received Message
            for eachMessage in receive_queue:
                dbconn.insert_recv(**eachMessage)
                print("Message Received: " + str(eachMessage))

        if send_queue:
            for eachMessage in send_queue:
                stat = s.send(eachMessage[1], eachMessage[2])
                if stat:
                    print("Success Send: " + str(eachMessage))
                    dbconn.confirm_send(eachMessage, str(time.time()).split('.')[0])
                else:
                    print("Fail to Send: " + str(eachMessage))

        conn.commit()
        time.sleep(5)
