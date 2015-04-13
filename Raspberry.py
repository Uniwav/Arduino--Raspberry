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

        time.sleep(5)    


    data = ""
    buff = ""

    while boolean == False:
    
        try:
            arduino.flush()
            data = arduino.readline()
            arduino.flush()
            arduino.write(str('K'))
            pass

        except IOError:
            boolean = True

        #print data

        if data != "99" and data != buff and buff != "":

            with open('data.txt', 'a') as fichier:

                fichier.write("\n")
                fichier.write(str(buff[0:len(buff)]))

            fichier.close()

        buff = data

        time.sleep(10)
