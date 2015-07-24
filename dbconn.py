#!/usr/bin/env python3

import mysql
from mysql import connector
import time

def query_send():
    try:
        db_cursor.execute("SELECT * FROM sms.tosend")
    except Exception as e:
        print(e.what())
        return False
    return db_cursor.fetchall()
    # [(mid(int), 'phone', 'message'), (), (), ...]



def confirm_send(sms, time):
    # tm = str(time.time()).split('.')[0]
    try:
        db_cursor.execute("SELECT * FROM sms.tosend WHERE mid = " + str(sms[0]))
        dt = db_cursor.fetchone()
        if not dt:
            return False
        db_cursor.execute("INSERT INTO sms.sent (phone, message, time) VALUES(%s, %s, %s)", (sms[1], sms[2], time))
        db_cursor.execute("DELETE FROM sms.tosend WHERE mid = " + str(dt[0]))
    except Exception as e:
        print(e.what())
        return False

    return True

def insert_recv(phone, time, message):
    try:
        db_cursor.execute("INSERT INTO sms.toread (phone, message, time) VALUES (%s, %s, %s)", (phone, message, time))
    except Exception as e:
        print(e.what())
        return False
    return True

# call by server
def insert_tosend(phone, message):
    try:
        db_cursor.execute("INSERT INTO sms.tosend (phone, message) VALUES (%s, %s)", (phone, message))
    except Exception as e:
        print(e.what())
        return False
    return True

"""
def threadcall():
    try:
        db_conn = mysql.connector.connect(host = 'localhost', user = 'root', password = 'uniquestudio')
        db_cursor = db_conn.cursor()
    except Exception as e:
        print("Fail to connect")
        exit()
    print("Successful connect to mysql server")

    while True:
        time.sleep(5)

        #log
        if not flag_sending:
            send_queue = query_send()
        # [(mid(int), 'phone', 'message'), (), (), ...]

        if not flag_reading:
            for eachSMS in receive_queue:
"""

if __name__ == "__main__":
    global flag_sending
    global flag_loading
    global send_queue
    global receive_queue
    send_queue = []
    receive_queue = []

    try:
        db_conn = mysql.connector.connect(host = 'localhost', user = 'root', password = 'uniquestudio')
        db_cursor = db_conn.cursor()
    except Exception as e:
        print(e.what())
        print("Fail to connect to server")

    insert_recv("+8618202725107", "dbtest", str(time.time()).split('.')[0])
    insert_tosend("+8618202725107", "dbtest2")
    send_queue = query_send()
    print(send_queue)

    for x in send_queue:
        print(x)
        confirm_send(x, str(time.time()).split('.')[0])
        time.sleep(1)
