#!/usr/bin/env python3

import mysql
from mysql import connector


def query_send():
    n = db_cursor.execute("SELECT * FROM tosend")
    new_dat = []
    for row in db_cursor.fetchall():
        print(row)
        for r in row:
            new_dat.append(r)
        send_queue.append(new_dat)
        new_dat = []

def confirm_send():
    pass

def insert_recv():
    pass

if __name__ == "__main__":
    global send_queue = []
    global receive_queue = []

    db_conn = mysql.connector.connect(host = 'localhost', user = 'root', password = 'uniquestudio')
    db_cursor = db_conn.cursor()

    try:
        db_conn.select_db('sms')
    except Exception as e:
        # init()
        print("Fail to connect to server")
        exit()
