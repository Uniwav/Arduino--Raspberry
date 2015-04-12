#!/usr/bin/python
# -*-coding:utf-8 -*


import serial
import time


usb = '/dev/ttyUSB0'
arduino = serial.Serial(usb, 9600, timeout = 5)


donnees = ""
buff = ""


while 1:

    arduino.flush()
    donnees = arduino.readline()
    arduino.flush()
    arduino.write(str('K'))

    #print donnees

    if donnees != "99" and donnees != buff and buff != "":

        with open('data.txt', 'a') as fichier:

            fichier.write("\n")
            fichier.write(str(buff[0:len(buff)]))

        fichier.close()

    buff = donnees

    time.sleep(10)
    
