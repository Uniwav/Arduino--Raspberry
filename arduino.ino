#include "Wire.h"
#include "LCD4884.h"
#include "Adafruit_BMP085.h"
#include "TinyDHT.h"
#include "DS1302.h"

#define DHT_P 10

#define LUM A3
#define SDA A4
#define SCL A5


#define TAILLEBUFFER 38
#define DELAYING     30


Adafruit_BMP085 bmp;
ds1302_struct rtc;
DHT dht(DHT_P, DHT11);


typedef struct
{
	float temp;
	float hygro;
	float pressure;
	float luminosity;

	char tempX[5];
	char hygroX[3];
	char pressureX[7];
	char luminosityX[3];

} VARIABLES;



void setup()
{
	Serial.begin(9600);
	lcd.init();
	lcd.clear();
	lcd.turnBacklightOn(false);

	dht.begin();
	while(!bmp.begin())
	{
		Serial.println("Error with BMP085 !");
		delay(1500);
	}
}


void loop()
{
	VARIABLES meteo;

	char buffer[TAILLEBUFFER];
	short int charLenght = 0;

	unsigned long int tempsAttendu = 0;

	for( ; ; delay(10000))
	{
		meteo.temp = (bmp.readTemperature() + dht.readTemperature(0)) / 2.0;
		meteo.hygro = dht.readHumidity();
		meteo.pressure = (bmp.readPressure() / 100.0);
		meteo.luminosity = (5.0 * analogRead(LUM)) / 1023.0;

		DS1302_clock_burst_read((uint8_t *) &rtc);

		sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d %s %s %s %s", \
	    	bcd2bin(rtc.Date10, rtc.Date), \
	    	bcd2bin(rtc.Month10, rtc.Month), \
	    	2000 + bcd2bin(rtc.Year10, rtc.Year), \
	    	bcd2bin(rtc.h24.Hour10, rtc.h24.Hour), \
	    	bcd2bin(rtc.Minutes10, rtc.Minutes), \
	    	bcd2bin(rtc.Seconds10, rtc.Seconds), \
	    	dtostrf(meteo.temp, 4, 1, meteo.tempX), \
	    	dtostrf(meteo.hygro, 2, 0, meteo.hygroX), \
	    	dtostrf(meteo.pressure, 6, 1, meteo.pressureX), \
	    	dtostrf(meteo.luminosity, 2, 0, meteo.luminosityX));

		ecran(&meteo);

		charLenght = Serial.write(buffer);
		Serial.flush();

		if(charLenght != TAILLEBUFFER  - 1)
		{
			Serial.write("\n99");
			Serial.flush();
		}


		tempsAttendu = millis();

		while(Serial.read() != 'K')
		{
			if((millis() - tempsAttendu) / 1000 <= DELAYING)
			{
				delay(100);
			}
			else
			{
				delay(10000);
			}
		}
	}
}


void ecran(VARIABLES *meteo)
{
	lcd.clear();
	lcd.turnBacklightOn(true);

	lcd.writeString(0, 0, meteo->tempX, MENU_NORMAL);
	lcd.writeString(1, 0, meteo->hygroX, MENU_NORMAL);
	lcd.writeString(2, 0, meteo->pressureX, MENU_NORMAL);
	lcd.writeString(3, 0, meteo->tempX, MENU_NORMAL);

	delay(3000);

	lcd.turnBacklightOn(false);
}
