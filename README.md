# [Uniwav](http://uniwav.eu/)

> The part you can build @home !

This repository contains the home system (composed by an _Arduino_ and a _Raspberry Pi_ inter-connected).

The _Arduino_ fetches some data from its environment, and send them to the _Raspberry_ through a serial communication (_USB_ in this case).

The _Raspberry_ may afterwards push these data on a web-server with `cron` and `rsync`.

On the _Arduino_ board, we used these libraries for each respective module :

* [LCD4884](https://gist.github.com/HorlogeSkynet/1df491c2ded935be178c9c6b2a026201)  
* [DS1302](https://gist.github.com/HorlogeSkynet/2d767c3af4862580c6287aa7901cf031)  
* [BMP085](https://github.com/adafruit/Adafruit-BMP085-Library)  
* [DHT11 KY015](https://github.com/adafruit/TinyDHT)  

On the _Raspberry_ runs a _Python_ script, and you'll need the [Serial](https://pypi.python.org/pypi/pyserial) library (`pip3 install pyserial`).
