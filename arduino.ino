#include "Wire.h"
#include "Adafruit_BMP085.h"
#include "DS1302.h"
#include "TinyDHT.h"
#include "LCD4884.h"


#define DHT_P 10

#define LUM A3
#define SDA A4
#define SCL A5

#define BUFFERSIZE                  38
#define DELAYING_ACK         30 * 1000
#define DELAYING_SENDING 5 * 60 * 1000
#define DELAY_RELOAD         15 * 1000
#define WARMUP_SCREEN    2 * 60 * 1000


Adafruit_BMP085 bmp;
DS1302 rtc;
DHT dht(DHT_P, DHT11);


/////////////////////////////////////////////////////////////////////////////////////////////////////
#define NUM_MENU_ITEM  4

const char menuList[NUM_MENU_ITEM][NBCHAR_X] = {"Temperature", "Humidity", "Pressure", "Luminosity"};

void temperature(void);
void humidity(void);
void pressure(void);
void luminosity(void);

FONCTION menuFunction[NUM_MENU_ITEM] = {temperature, humidity, pressure, luminosity};

const char *projectName = "Uniwav:";
/////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct
{
    float temp;
    float hygro;
    float pressure;
    int luminosity;

    char tempX[5];
    char hygroX[3];
    char pressX[7];
    char lumX[2];

} Variables;


Variables variables;


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
	static unsigned long int schedule = 0;
	static unsigned long int printDot = 0;
	static unsigned long int reload   = 0;
	static bool reload_bool = false;

	// Reprint time on menu
	if(millis() - reload >= DELAY_RELOAD || reload_bool)
	{
		printTime();
		reload = millis();
		reload_bool = false;
	}

	lcd.browseMenu(menuList, menuFunction);

	// Send data
	if(millis() - schedule >= (long)DELAYING_SENDING || lcd.getLongPress() == true)
	{
		sendData();
		schedule = millis();
		reload_bool = true;
	}

	// Warm-up screen
	if(millis() - printDot >= (long)WARMUP_SCREEN)
	{
		lcd.clear();
		for(short int i = 0, j; i <= 5; i++)
		{
			for(j = 0; j < NBCHAR_X; j++)
			{
				if(i == 2 && j == 14)
				{
					lcd.clear();
				}
				lcd.writeString(j * NB_PIX_X, i, ".", MENU_NORMAL);
				lcd.writeString((NBCHAR_X - 1 - j) * NB_PIX_X, 5 - i, ".", MENU_HIGHLIGHT);
				delay(75);
			}
		}

		lcd.clear();
		printDot = millis();
		reload_bool = true;
	}
}


void updateData(void)
{
	variables.temp = (bmp.readTemperature() + dht.readTemperature(0)) / 2.0;
	variables.hygro = dht.readHumidity();
	variables.pressure = (bmp.readPressure() / 100.0);
	variables.luminosity = scaleLumimosity((int)analogRead(LUM));

	dtostrf(variables.temp, 4, 1, variables.tempX);
	dtostrf(variables.hygro, 2, 0, variables.hygroX);
	dtostrf(variables.pressure, 6, 1, variables.pressX);

	if(variables.luminosity >= 10)
	{
		sprintf(variables.lumX, "%02d", variables.luminosity);
	}
	else
	{
		sprintf(variables.lumX, " %d", variables.luminosity);
	}
}


void sendData(void)
{
	char buffer[BUFFERSIZE];
	short int charLenght = 0;

	updateData();

	lcd.turnBacklightOn(true);
	lcd.clear();
	lcd.writeString(CENTER("Sending"), 1, "Sending", MENU_NORMAL);
	lcd.writeString(CENTER("data..."), 2, "data...", MENU_NORMAL);
	delay(1000);

	DS1302_clock_burst_read((uint8_t *) &rtc);

	sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d %s %s %s %s", \
    	bcd2bin(rtc.Date10, rtc.Date), \
    	bcd2bin(rtc.Month10, rtc.Month), \
    	2000 + bcd2bin(rtc.Year10, rtc.Year), \
    	bcd2bin(rtc.h24.Hour10, rtc.h24.Hour), \
    	bcd2bin(rtc.Minutes10, rtc.Minutes), \
    	bcd2bin(rtc.Seconds10, rtc.Seconds), \
    	variables.tempX, \
    	variables.hygroX, \
    	variables.pressX, \
    	variables.lumX);

	charLenght = Serial.write(buffer);
	Serial.flush();

	if(charLenght != BUFFERSIZE - 1) // Buffer couldn't be written entirely
	{
		Serial.write("\nError");
		Serial.flush();

		lcd.clear();
		lcd.writeString(CENTER("Data can't"), 1, "Data can't", MENU_NORMAL);
		lcd.writeString(CENTER("be sent..."), 2, "be sent...", MENU_NORMAL);
		delay(1000);

		lcd.turnBacklightOn(false);
	}

	else
	{
		unsigned long int timeWaited = millis();

		lcd.clear();
		lcd.writeString(CENTER("Waiting"), 1, "Waiting", MENU_NORMAL);
		lcd.writeString(CENTER("for Pi..."), 2, "for Pi...", MENU_NORMAL);
		delay(500);

		while(Serial.read() != 'OK')
		{
			if((millis() - timeWaited) <= DELAYING_ACK)
			{
				delay(100);
			}

			else
			{
				lcd.turnBacklightOn(false);
				delay(10000);
			}
		}

		lcd.clear();
		lcd.writeString(CENTER("Data sent."), 1, "Data sent.", MENU_NORMAL);
		delay(1500);
		lcd.clear();
		lcd.turnBacklightOn(false);
	}
}


void printTime(void)
{
	char timeBuffer[NBCHAR_X];

	DS1302_clock_burst_read((uint8_t *) &rtc);

	sprintf(timeBuffer, "%02d/%02d/%02d %02d:%02d", \
    	bcd2bin(rtc.Date10, rtc.Date), \
    	bcd2bin(rtc.Month10, rtc.Month), \
    	bcd2bin(rtc.Year10, rtc.Year), \
    	bcd2bin(rtc.h24.Hour10, rtc.h24.Hour), \
    	bcd2bin(rtc.Minutes10, rtc.Minutes));

    lcd.showMenu(menuList);
    lcd.writeString(0, 1, timeBuffer, MENU_NORMAL);
}


void temperature(void)
{
	updateData();

	lcd.writeString(0, 1, menuList[0], MENU_NORMAL);
	lcd.writeString(11 * NB_PIX_X, 1, ":", MENU_NORMAL);

	lcd.writeStringBig((NBCHAR_X / 4) * NB_PIX_X, 2, variables.tempX, MENU_NORMAL);
	lcd.writeString(10 * NB_PIX_X, 4, "oC", MENU_NORMAL);
}


void humidity(void)
{
	updateData();

	lcd.writeString(0, 1, menuList[1], MENU_NORMAL);
	lcd.writeString(8 * NB_PIX_X, 1, ":", MENU_NORMAL);

	lcd.writeStringBig(4 * NB_PIX_X, 2, variables.hygroX, MENU_NORMAL);
	lcd.writeString(9 * NB_PIX_X, 4, "%", MENU_NORMAL);
}


void pressure(void)
{
	updateData();

	lcd.writeString(0, 1, menuList[2], MENU_NORMAL);
	lcd.writeString(8 * NB_PIX_X, 1, ":", MENU_NORMAL);

	lcd.writeStringBig(NB_PIX_X, 2, variables.pressX, MENU_NORMAL);
	lcd.writeString(12 * NB_PIX_X, 4, "Pa", MENU_NORMAL);
}


void luminosity(void)
{
	updateData();

	lcd.writeString(0, 1, menuList[3], MENU_NORMAL);
	lcd.writeString(10 * NB_PIX_X, 1, ":", MENU_NORMAL);

	lcd.writeStringBig(5 * NB_PIX_X, 2, variables.lumX, MENU_NORMAL);
}


int scaleLumimosity(int scale)
{
	if(scale >= 1024)
		return 22;
	else if(scale >= 1000)
		return 21;
	else if(scale >=  900)
		return 20;
	else if(scale >=  750)
		return 19;
	else if(scale >=  700)
		return 18;
	else if(scale >=  650)
		return 17;
	else if(scale >=  600)
		return 16;
	else if(scale >=  500)
		return 15;
	else if(scale >=  400)
		return 14;
	else if(scale >=  200)
		return 13;
	else if(scale >=  175)
		return 12;
	else if(scale >=  100)
		return 11;
	else if(scale >=   50)
		return 10;
	else if(scale >=   25)
		return  9;
	else if(scale >=   20)
		return  8;
	else if(scale >=   15)
		return  7;
	else if(scale >=   10)
		return  6;
	else if(scale >=    9)
		return  5;
	else if(scale >=    7)
		return  4;
	else if(scale >=    5)
		return  3;
	else if(scale >=    3)
		return  2;
	else if(scale >=    1)
		return  1;
	else
		return  0;
}
