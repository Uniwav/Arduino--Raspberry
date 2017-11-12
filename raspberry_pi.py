#!/usr/bin/env python3


import serial
import time


USB = '/dev/ttyUSB0'


# This program is not supposed to stop ;)
while True:

    # Here we'll loop while we ARE NOT connected to...
    # ... the Arduino through the Serial port
    while True:

        try:
            arduino = serial.Serial(USB, 9600, timeout=5)
            break

        except OSError:
            time.sleep(10)

    data = ''
    temp = ''
    arduino.flushInput()

    # Here we'll loop again while we ARE connected to the Arduino !
    while True:

        try:
            data = arduino.readline()
            arduino.write(str('OK'))
            arduino.flush()

        except IOError:
            break

        # print data

        # If these data exist, and are different than the previous iteration...
        if data != '' and data != temp and data != 'Error':

            # ... let's save 'em !
            with open('data.txt', 'a') as fichier:
                fichier.write('\n' + data)

        temp = data

        time.sleep(5)
