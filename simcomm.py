#!/usr/bin/env python3

import serial
import time
import binascii

def class connector:
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
        ucsphone = binascii.hexlify(str(phone).encode("utf-16be"))
        ucsmessage = binascii.hexlify(str(message).encode("utf-16be"))
        command = "AT+CMGS=\"".encode() + ucsphone + "\"\r\n".encode() + ucsmessage + b"\x1A"
        pass

    def receive(self):
        pass

    def __del__(self):
        self.se.close()
