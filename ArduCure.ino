 //Libraries utilized
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
#include <SD.h>
#include <HX711.h>

//Component pins
//SD card pins:
const int chipSelect=8;
//DHT pin:
const int dhtPin=10;
// relay pins:
const int dehumidifier=4;
const int humidifier=5;
const int heater=6;
const int cooler=7;
//scale callibration coefficient
const float scaleCal=1;

//global Data constants
float weight;
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

//Creating conponent opbjects
DHT dht;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address
HX711 scale(A1, A0, 64);

//functions:

//returns seconds since start up
unsigned long now(){
  return (millis()+500 - (millis()+500) % 1000)/1000;
  }
  
void initializeLoadCell(){
 lcd.clear();
 lcd.setCursor(0,0); 
 lcd.print("Scale Cal Coeff.");
 lcd.setCursor(1,1);
 lcd.print(scaleCal,2);
 scale.set_scale(scaleCal);
 delay(5000);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Setting tare");
 delay(5000);
 scale.tare(30);
 lcd.clear();
 lcd.setCursor(1,1);
 lcd.print("Tare set"); 
 delay(5000);
}

void readLoadCell(){
  weight=scale.get_units(10);
}
  
void readDHT(){
  humidity=0;
  temperature=0;
  int reads=0;
  int whileCount=0;
  //averaging three readings, for better accuracy
  while(reads<3){
    whileCount++;
    delay(dht.getMinimumSamplingPeriod()+10);
    //evaluating sensor status
    dhtStatus=dht.getStatus();
    dhtStatusString=dht.getStatusString();
    //if status is ok sensor is read.
    if(!dht.getStatus()){
      reads++;
      humidity +=dht.getHumidity();
      temperature += dht.getTemperature();
    }
    //if sensor loop is run more than 5 times the function is exited
    if(whileCount>5){
      Serial.println("Sensor failed to read");
      dht.setup(dhtPin);// reinitilizing sensor
      dhtStatus=1; //sensor failed
      return;
    }
  }
  //averaging results
    humidity = humidity/3;
    temperature = temperature/3;
}

void displayData(){
  //if readings are comming through from the dht
  if(!dhtStatus){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Rel. Hum., Temp.");  
    lcd.setCursor(4,1);
    lcd.print(humidity,1);
    lcd.setCursor(8,1);
    lcd.print("%");
    lcd.setCursor(11,1);
    lcd.print(temperature,1);
    lcd.setCursor(15,1);
    lcd.print("C");
    delay(4000);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Weight");
    lcd.setCursor(2,1);
    lcd.print(weight,2);
    delay(1000);
  }else{
    lcd.clear();
    lcd.print(dhtStatusString);
  }
}

void writeData(){
  //SD Card initialization check
  if(!sdCardInitialized){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SD Card not");
    lcd.setCursor(0,1);
    lcd.print(" initialized");
    delay(1000);
    return;
  }
  //Opening the datafile
  File dataFile = SD.open(datalogName, FILE_WRITE);  
  //if file and data is available data is written to the file
  if(!dhtStatus && dataFile){
    Serial.print(now());
    dataFile.print(now());
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(temperature,2);
    dataFile.print(temperature,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(humidity,2);
    dataFile.print(humidity,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(weight,2);
    dataFile.print(weight,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(digitalRead(heater));
    dataFile.print(digitalRead(heater));
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(digitalRead(cooler));
    dataFile.print(digitalRead(cooler));
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(digitalRead(humidifier));
    dataFile.print(digitalRead(humidifier));
    Serial.print(" ");
    dataFile.print(" ");
    Serial.println(digitalRead(dehumidifier));
    dataFile.println(digitalRead(dehumidifier));
    dataFile.close();
  }else if(!dhtStatus && !dataFile){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No SD Card");
    Serial.println("No SD Card");
    SD.begin(chipSelect);
  }
}

void dataFileInitialization(){
  //String to write to line 1 in txt datafile
  char* dataHeader = "time Temp Hum Weigth Heater Cooler Hum'er deHum'er";  
  // open or create the datafile.
  Serial.println("initializing sd card");
  int i=1; //counter
  String datalogNameStr = "Data/data";
  datalogNameStr = datalogNameStr+i+".txt";
  datalogNameStr.toCharArray(datalogName,16);
  //cycling stored logfile names, untill a free name is reached
  while(SD.exists(datalogName)){
    i++;
    datalogNameStr = "Data/data";
    datalogNameStr = datalogNameStr+i+".txt";
    datalogNameStr.toCharArray(datalogName,16);
  }
  //Create the logfile
  File dataFile = SD.open(datalogName, FILE_WRITE); 
   // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataHeader);
    dataFile.close();
    //set initialization
    sdCardInitialized=true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SD Card");
    lcd.setCursor(0,1);
    lcd.print(" initialized");
    delay(1000);
  }else{
    //set initialization
    sdCardInitialized=false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SD Card not");
    lcd.setCursor(0,1);
    lcd.print(" initialized");
    delay(5000);
    lcd.clear();
  }
}

//Loading the control data if available on the SD card 
void loadControlParameters(){
  //Opening the datafile
  File dataFile = SD.open("Control/control.txt", FILE_READ);
  long controlTime=0;
  //if file is available
  if(dataFile){
    controlTime=dataFile.parseInt(); //First control time is loaded (usually zero)
    //cycling through the curing program until relevant stage is reached
    while(now()>=controlTime && dataFile.available()){
      targetTemp=dataFile.parseFloat();
      hystTemp=dataFile.parseFloat();
      targetHumidity=dataFile.parseFloat();
      hystHumidity=dataFile.parseFloat();
      controlTime=dataFile.parseInt(); //The controlTime of the next line
    }
  }else{
    //If a file isn't available restart the sd reader
    SD.begin(chipSelect);
  }
  dataFile.close();   
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Targets:");  
  lcd.setCursor(4,1);
  lcd.print(targetHumidity,1);
  lcd.setCursor(8,1);
  lcd.print("%");
  lcd.setCursor(11,1);
  lcd.print(targetTemp,1);
  lcd.setCursor(15,1);
  lcd.print("C");
}

void setRelaysLow(){
  digitalWrite(heater,LOW);
  digitalWrite(cooler,LOW);
  digitalWrite(humidifier,LOW);
  digitalWrite(dehumidifier,LOW);
}

int control(float reading,float target,float hyst){
//First a sensor check is performed
  int band;
  if(dhtStatus){
    //Control: all off
    band=-1;
    setRelaysLow();
    return band;
  }
  //too high
  if(reading>target+hyst){
    band=3;
  //upper band
  }else if(target<=reading && reading<=target+hyst){
    band=2;
  //lower band
  }else if(target-hyst<=reading && reading<target){
    band=1;
  //too high
  }else if(reading<target-hyst){
    band=0;
  }
  return band;
}
//Setting the realys according to the output of the control function
void setRelays(){
  //getting the inputs
  int tempBand=control(temperature,targetTemp,hystTemp);
  int humBand=control(humidity,targetHumidity,hystHumidity);
  //Checking status
  if(dhtStatus)return;
  
  //Heater
  if(tempBand==0){
    digitalWrite(heater,HIGH);
  }else if(tempBand==1 && digitalRead(heater)){
    digitalWrite(heater,HIGH);
  }else if(tempBand==2 && humBand>2){
    digitalWrite(heater,HIGH);
  }else{
    digitalWrite(heater,LOW);
  }
  
  //Cooler
  if(tempBand==3){
    digitalWrite(cooler,HIGH);
  }else if(tempBand==2&&digitalRead(cooler)){
    digitalWrite(cooler,HIGH);
  }else{
    digitalWrite(cooler,LOW);
  }
  
  //humidifier
  if(humBand==0){
    digitalWrite(humidifier,HIGH);
  }
  else if(humBand==1&&digitalRead(humidifier)){
    digitalWrite(humidifier,HIGH);
  }else{
    digitalWrite(humidifier,LOW);
  }
  
  //dehumidifier
  if(humBand==3){
    digitalWrite(dehumidifier,HIGH);
  }
  else if(humBand==2&&digitalRead(dehumidifier)){
    digitalWrite(dehumidifier,HIGH);
  }else{
    digitalWrite(dehumidifier,LOW);
  }
}

// the setup routine runs once when you press reset:
void setup(){
  Serial.begin(9600);
  delay(2000);  
  //initializing components
  lcd.init();
  lcd.backlight();
  dht.setup(dhtPin);
  SD.begin(chipSelect);
  initializeLoadCell();
  //relay initialization
  pinMode(dehumidifier, OUTPUT);
  pinMode(humidifier, OUTPUT);
  pinMode(cooler, OUTPUT);
  pinMode(heater, OUTPUT);
  //Datafile initialization
  dataFileInitialization(); 
  //Control initialization
  loadControlParameters();
}

// the loop routine runs over and over again forever:
void loop(){
  readLoadCell();
  readDHT();
  displayData();
  //temperature Control
  setRelays();
  saveCounter++;
  if(saveCounter==10){
    writeData();
    loadControlParameters();
    delay(2000); //saving data interfers with the dht
    saveCounter=0;
  }
}


