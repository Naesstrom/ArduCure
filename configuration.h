//Component pins
//SD card pins:
const int chipSelect=8;
//DHT pin:
const int dhtPin=49;
// relay pins:
const int dehumidifier=41;
const int humidifier=43;
const int heater=45;
const int cooler=47;
//scale callibration coefficient
const float scaleCal=1;
//ESP8266-07


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
const char* ssid     = "your-ssid";
const char* password = "your-password";

//Creating conponent opbjects
DHT dht;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address
HX711 scale1(A1, A0, 64);
HX711 scale2(A3, A2, 64);
HX711 scale3(A5, A4, 64);
HX711 scale4(A7, A6, 64);
HX711 scale5(A9, A8, 64);
HX711 scale6(A11, A10, 64);
HX711 scale7(A13, A12, 64);
HX711 scale8(A15, A14, 64);

