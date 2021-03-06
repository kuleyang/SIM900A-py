#!/usr/bin/env python3

import mysql
from mysql import connector

def init():
    try:
        cursor.execute("""CREATE DATABASE IF NOT EXISTS sms DEFAULT CHARSET utf8 COLLATE utf8_general_ci""")
        #conn.select_db("sms")
        cursor.execute("""
        CREATE TABLE `sms`.`tosend`(
        `mid` int NOT NULL UNIQUE AUTO_INCREMENT,
        `phone` varchar(20) NOT NULL,
        `message` varchar(200),
        PRIMARY KEY (`mid`),
        INDEX (`mid`)
        )ENGINE=InnoDB charset=utf8 collate=utf8_general_ci
        """)

        cursor.execute("""
        CREATE TABLE `sms`.`sent`(
        `mid` int NOT NULL UNIQUE AUTO_INCREMENT,
        `phone` varchar(20) NOT NULL,
        `message` varchar(200),
        `time` varchar(11),
        PRIMARY KEY (`mid`),
        INDEX (`mid`)
        )ENGINE=InnoDB charset=utf8 collate=utf8_general_ci
        """)

        cursor.execute("""
        CREATE TABLE `sms`.`toread`(
        `mid` int NOT NULL UNIQUE AUTO_INCREMENT,
        `phone` varchar(20) NOT NULL,
        `message` varchar(200),
        `time` varchar(11),
        PRIMARY KEY (`mid`),
        INDEX (`mid`)
        )ENGINE=InnoDB charset=utf8 collate=utf8_general_ci
        """)

        cursor.execute("""
        CREATE TABLE `sms`.`read`(
        `mid` int NOT NULL UNIQUE AUTO_INCREMENT,
        `phone` varchar(20) NOT NULL,
        `message` varchar(200),
        `time` varchar(11),
        PRIMARY KEY (`mid`),
        INDEX (`mid`)
        )ENGINE=InnoDB charset=utf8 collate=utf8_general_ci
        """)

        conn.commit()
    except:
        conn.rollback()

def test():
    cursor.execute('INSERT INTO sms.toread (phone, message, time) values (%s, %s, %s)', ("18202725107", "Demo", "1436094706"))
    cursor.execute('INSERT INTO sms.read (phone, message, time) values (%s, %s, %s)', ("18202725107", "Demo", "1436094706"))
    cursor.execute('INSERT INTO sms.sent (phone, message, time) values (%s, %s, %s)', ("18202725107", "Demo", "1436094706"))
    cursor.execute('INSERT INTO sms.tosend (phone, message) values (%s, %s)', ("18202725107", "Demo"))
    conn.commit()


if __name__ == '__main__':
    conn = mysql.connector.connect(host='localhost', user='root', password='ow02mt')
    cursor = conn.cursor()

    try:
        test()
    except Exception as e:
        print("It seems database not initialized")
        init()
        test()
