#include "Wire.h"
#include "Adafruit_BMP085.h"
#include "DS1302.h"
#include "TinyDHT.h"
#include "LCD4884.h"


#define DHT_P 10

#define LUM A3
#define SDA A4
#define SCL A5

#define TAILLEBUFFER 		 	   38
#define DELAYING_ACK	 	  30*1000
#define DELAYING_SENDING   30*60*1000
#define DELAY_AFTER_START   1*60*1000



Adafruit_BMP085 bmp;
DS1302 rtc;
DHT dht(DHT_P, DHT11);



/////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUM_MENU_ITEM  4

char menuList[NUM_MENU_ITEM][NBCHAR_X] = {"Temperature", "Humidity", "Pressure", "Luminosity"};

void temperature(void);
void humidity(void);
void pressure(void);
void luminosity(void);

FONCTION menuFunction[NUM_MENU_ITEM] = {temperature, humidity, pressure, luminosity};

char *projectName = "Uniwav:";
/////////////////////////////////////////////////////////////////////////////////////////////////////



VARIABLES env;



void setup()
{
	Serial.begin(9600);

	lcd.initClass(projectName, NUM_MENU_ITEM);
	lcd.showMenu(menuList);

	dht.begin();
	while(!bmp.begin())
	{
		//Serial.println("Error with BMP085 !");
		delay(1500);
	}
}


void loop()
{
	static unsigned long int schedule = DELAY_AFTER_START;


	//Menu
	lcd.browseMenu(menuList, menuFunction);


	//Envoie données
	if(millis() - schedule >= DELAYING_SENDING || lcd.getLongPress() == true)
	{
		sendData();
		schedule = millis();
	}
}



void updateData()
{
	env.temp = (bmp.readTemperature() + dht.readTemperature(0)) / 2.0;
	env.hygro = dht.readHumidity();
	env.pressure = (bmp.readPressure() / 100.0);
	env.luminosity = (5.0 * analogRead(LUM)) / 1023.0;

	dtostrf(env.temp, 4, 1, env.tempX);
	dtostrf(env.hygro, 2, 0, env.hygroX);
	dtostrf(env.pressure, 6, 1, env.pressX);
	dtostrf(env.luminosity, 2, 0, env.lumX);
}


void sendData()
{
	updateData();

	lcd.turnBacklightOn(true);
	lcd.clear();
	lcd.writeString(CENTER("Sending"), 1, "Sending", MENU_NORMAL);
	lcd.writeString(CENTER("data..."), 2, "data...", MENU_NORMAL);

	char buffer[TAILLEBUFFER];
	short int charLenght = 0;

	unsigned long int timeWaited = 0;

	for( ; ; delay(10000))
	{
		DS1302_clock_burst_read((uint8_t *) &rtc);

		sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d %s %s %s %s", \
	    	bcd2bin(rtc.Date10, rtc.Date), \
	    	bcd2bin(rtc.Month10, rtc.Month), \
	    	2000 + bcd2bin(rtc.Year10, rtc.Year), \
	    	bcd2bin(rtc.h24.Hour10, rtc.h24.Hour), \
	    	bcd2bin(rtc.Minutes10, rtc.Minutes), \
	    	bcd2bin(rtc.Seconds10, rtc.Seconds), \
	    	env.tempX, \
	    	env.hygroX, \
	    	env.pressX, \
	    	env.lumX);

		charLenght = Serial.write(buffer);
		Serial.flush();

		if(charLenght != TAILLEBUFFER  - 1)
		{
			Serial.write("\n99");
			Serial.flush();
		}

		tempsAttendu = millis();

		lcd.clear();
		lcd.writeString(CENTER("Waiting"), 1, "Waiting", MENU_NORMAL);
		lcd.writeString(CENTER("for Pi..."), 2, "for Pi...", MENU_NORMAL);
		delay(500);

		while(Serial.read() != 'K')
		{
			if((millis() - timeWaited) <= DELAYING_ACK)
			{
				delay(100);
			}
			else
			{
				delay(10000);
			}
		}

		lcd.clear();
		lcd.writeString(CENTER("Data sent."), 1, "Data sent.", MENU_NORMAL);
		delay(1500);
		lcd.turnBacklightOn(false);
		lcd.clear();
	}
}


void temperature(void)
{
	updateData();

	lcd.writeString(0, 1, menuList[0], MENU_NORMAL);
	lcd.writeString(11 * NB_PIX_X, 1, ":", MENU_NORMAL);

	lcd.writeStringBig((NBCHAR_X / 4) * NB_PIX_X, 2, env.tempX, MENU_NORMAL);
	lcd.writeString(10 * NB_PIX_X, 4, "oC", MENU_NORMAL);
}


void humidity(void)
{
	updateData();

	lcd.writeString(0, 1, menuList[1], MENU_NORMAL);
	lcd.writeString(8 * NB_PIX_X, 1, ":", MENU_NORMAL);

	lcd.writeStringBig(4 * NB_PIX_X, 2, env.hygroX, MENU_NORMAL);
	lcd.writeString(9 * NB_PIX_X, 4, "%", MENU_NORMAL);
}


void pressure(void)
{
	updateData();

	lcd.writeString(0, 1, menuList[2], MENU_NORMAL);
	lcd.writeString(8 * NB_PIX_X, 1, ":", MENU_NORMAL);

	lcd.writeStringBig(NB_PIX_X, 2, env.pressX, MENU_NORMAL);
	lcd.writeString(12 * NB_PIX_X, 4, "Pa", MENU_NORMAL);
}


void luminosity(void)
{
	updateData();

	lcd.writeString(0, 1, menuList[3], MENU_NORMAL);
	lcd.writeString(10 * NB_PIX_X, 1, ":", MENU_NORMAL);

	lcd.writeStringBig(4 * NB_PIX_X, 2, env.lumX, MENU_NORMAL);
	lcd.writeString(9 * NB_PIX_X, 4, "%", MENU_NORMAL);
}
