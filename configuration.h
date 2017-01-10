#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>;

//Set SSID and password for your WIFI Connection
//ESP8266-07
const char* ssid     = "your-ssid";
const char* password = "your-password";

//Component pins
//SD card pins:
const int chipSelect=8;
//DHT pin:
//const int dhtPin=49;
#define DHTPIN 2            // what digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22       // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21     // DHT 21 (AM2301)


// relay pins:
const int dehumidifier=41;
const int humidifier=43;
const int heater=45;
const int cooler=47;


//scale callibration coefficient
const float scaleCal=1;


//global Data constants
float weight1;
float weight2;
float weight3;
float weight4;
float weight5;
float weight6;
float weight7;
float weight8;
boolean dhtStatus;
String dhtStatusString;
float humidity;
float temperature;
boolean sdCardInitialized;
char datalogName[16];
int saveCounter=0;
//Control parameters
float targetTemp=27.5;
float hystTemp=2;
float targetHumidity=50;
float hystHumidity=10;

//Creating component opbjects
//DHT dht;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address
HX711 scale1(A1, A0);
HX711 scale2(A3, A2);
HX711 scale3(A5, A4);
HX711 scale4(A7, A6);
HX711 scale5(A9, A8);
HX711 scale6(A11, A10);
HX711 scale7(A13, A12);
HX711 scale8(A15, A14);

#endif //__CONFIGURATION_H

