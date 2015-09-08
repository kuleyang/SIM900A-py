#!/usr/bin/env python3

import serial
import time
import binascii
import mysql
from mysql import connector
import logging

import dbconn

class connector(object):
    def __init__(self, port = "/dev/ttyUSB0", bound = 115200, timeout = 1):
        self.logger = logging.getLogger()
        try:
            self.se = serial.Serial(port, bound,
                timeout = timeout,
                parity = serial.PARITY_NONE,
                stopbits = serial.STOPBITS_ONE,
                bytesize = serial.EIGHTBITS)
        except:
            print("Fail to open device " + port + "\nAre you ROOT?")
            logger.critical("Fail to open device " + port)
            exit()

        self.se.write("AT\r\n".encode())
        time.sleep(0.2)
        self.se.write("AT\r\n".encode())
        time.sleep(0.2)
        self.se.flushInput()

        self.se.write("AT+CPIN?\r\n".encode())
        time.sleep(0.2)
        if self.se.readall().decode("utf8").find("READY") < 0:
            logger.critical("SIM Init Fail, system exit")
            exit()

        self.se.write("AT+CMGF=1\r\n".encode())
        time.sleep(0.2)
        if self.se.readall().decode("utf8").find("OK") < 0:
            logger.critical("SIM Init Fail, system exit")
            exit()

        self.se.write("AT+CSCS=\"UCS2\"\r\n".encode())
        self.se.write("AT+GSMBUSY=1\r\n".encode())
        time.sleep(0.2)
        if self.se.readall().decode("utf8").find("ERROR") > -1:
            logger.critical("SIM Init Fail, system exit")
            exit()

    def __del__(self):
        self.se.close()
        logger.info("Serial Port Close")

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
        logger.error("Send Fail, Trace:[  " + ret + "  ]")
        return False

    def receive(self, delete = True):
        self.se.flushInput()
        self.se.write("AT+CMGL=\"ALL\"\r\n".encode())
        time.sleep(0.1)
        cmgl = self.se.readall()

        # print(cmgl)

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




if __name__ == "__main__":
    logger = logging.getLogger()
    logfile = "sms.log"
    handler = logging.FileHandler(logfile)
    formatter =  logging.Formatter('[%(asctime)s][%(levelname)s]: %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    logger.setLevel(logging.INFO)

    try:
        db_conn = mysql.connector.connect(host = 'localhost', user = 'root', password = 'ow02mt')
        db_cursor = db_conn.cursor()
    except Exception as e:
        print(e)
        print("Fail to connect to server")
        logger.error("Fail to connect to mysql server: " + str(e))
        exit(0)

    logger.info("Connect Established")
    s = connector()

    print("Init Finished")
    logger.info("Init Finished")

    while True:
        logger.debug("New Round")
        receive_queue = s.receive()
        # print("Recv: " + str(receive_queue))
        logger.debug("receive queue: len " + str(len(receive_queue)))
        send_queue = dbconn.query_send(db_cursor)
        #print("Send: " + str(send_queue))
        logger.debug("send queue: len " + str(len(send_queue)))

        if receive_queue:
            #Received Message
            for eachMessage in receive_queue:
                dbconn.insert_recv(db_cursor, eachMessage)
                # print("Message Received: " + str(eachMessage))
                logger.info("SMS Received: " + str(eachMessage))

        if send_queue:
            for eachMessage in send_queue:
                stat = s.send(eachMessage[1], eachMessage[2])
                if stat:
                    # print("Success Send: " + str(eachMessage))
                    logger.info("SMS Send Successful: " + str(eachMessage))
                    dbconn.confirm_send(db_cursor, eachMessage, str(time.time()).split('.')[0])
                else:
                    # print("Fail to Send: " + str(eachMessage))
                    logger.warn("SMS Send Fail! Message:" + str(eachMessage))
                # in case isp stop service
                time.sleep(5)

        db_conn.commit()
        time.sleep(5)
