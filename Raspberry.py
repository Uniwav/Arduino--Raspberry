#!/usr/bin/python
# -*-coding:utf-8 -*


import serial
import time


USB = '/dev/ttyUSB0'


while 1:

    boolean = True

    while boolean == True:
    
        try:
            arduino = serial.Serial(USB, 9600, timeout = 5)
            boolean = False
            pass

        except OSError:
            boolean = True

        time.sleep(10)


    data = ""
    buff = ""
    arduino.flushInput()

    while boolean == False:
    
        try:
            data = arduino.readline()
            arduino.write(str('K'))
            arduino.flush()
            pass

        except IOError:
            boolean = True

        #print data

        if data != buff and buff != "" and data != "99":

            with open('data.txt', 'a') as fichier:

                fichier.write("\n")
                fichier.write(str(buff[0:len(buff)]))
                fichier.flush()

            fichier.close()

        buff = data

        time.sleep(5)
