# [Uniwav](http://uniwav.eu/) - Technical part

This is the *Horloge*'s Part with *Arduino* → *Raspberry* communication, and the data writing system.
All the sending toward web server is managed by ``rsync `` and ``cron `` on *Raspberry*.  

The components used in the system are running with these libraries:  
  
[LCD4884](https://github.com/HorlogeSkynet/LCD4884)  
[DS1302](https://github.com/HorlogeSkynet/DS1302)  
[BMP085](https://github.com/adafruit/Adafruit-BMP085-Library)  
[DHT11 KY015](https://github.com/adafruit/TinyDHT)  
