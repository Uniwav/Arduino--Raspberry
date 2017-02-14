# [Uniwav](http://uniwav.eu/) - Technical part

This is the *Horloge*'s part with *Arduino* → *Raspberry* communication, and the data writing system.  
All the sending system towards a web server is managed by `rsync` and `cron` on the *Raspberry*.

On *C / C++* side, components used in the system are running with these libraries:

[LCD4884](https://gist.github.com/HorlogeSkynet/1df491c2ded935be178c9c6b2a026201)  
[DS1302](https://gist.github.com/HorlogeSkynet/2d767c3af4862580c6287aa7901cf031)  
[BMP085](https://github.com/adafruit/Adafruit-BMP085-Library)  
[DHT11 KY015](https://github.com/adafruit/TinyDHT)  

On the *Python* script, we use the `Serial` library, available [here](https://pypi.python.org/pypi/pyserial).
