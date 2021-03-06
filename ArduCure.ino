#include <SimpleDHT.h>
#include <DHT.h>

//Libraries utilized
//DS3231 can be downloaded from http://www.rinkydinkelectronics.com/library.php?id=73
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <Q2HX711.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <DS3231.h>
//This is the hx711 library available at https://github.com/bogde/HX711
#include <HX711.h>

#include "configuration.h";
#include "WiFi.h";

//functions:
DHT dht(DHTPIN, DHTTYPE);

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
 scale1.set_scale(scaleCal);
 delay(5000);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Setting tare");
 delay(5000);
 scale1.tare(30);
 lcd.clear();
 lcd.setCursor(1,1);
 lcd.print("Tare set");
 delay(5000);
}

void readLoadCell(){
  weight1=scale1.get_units(10);
}

#include "read_dht.h";

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
    lcd.print(weight1,2);
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
  //Print temperature and Humidity
    Serial.print(temperature,2);
    dataFile.print(temperature,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(humidity,2);
    dataFile.print(humidity,2);
    Serial.print(" ");
    dataFile.print(" ");
  //Print weightsensors 1-8
    Serial.print(weight1,2);
    dataFile.print(weight1,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(weight2,2);
    dataFile.print(weight2,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(weight3,2);
    dataFile.print(weight3,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(weight4,2);
    dataFile.print(weight4,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(weight5,2);
    dataFile.print(weight5,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(weight6,2);
    dataFile.print(weight6,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(weight7,2);
    dataFile.print(weight7,2);
    Serial.print(" ");
    dataFile.print(" ");
    Serial.print(weight8,2);
    dataFile.print(weight8,2);
    Serial.print(" ");
    dataFile.print(" ");
  //Print relaystatus
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
  char* dataHeader = "time Temp Hum Weigth_1 Weight_2 Weight_3 Weight_4 Weight_5 Weight_6 Weight_7 Weight_8 Heater Cooler Hum'er deHum'er";
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
  dht.begin();
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

  //Send data to InfluxDb (https://goo.gl/AvktxY)
  void loop() {
  String line, temperature;

  // wait 1 second
  delay(1000);
  
  // get the current temperature from the sensor, to 2 decimal places
  temperature = String(readDHT() 2);

  // concatenate the temperature into the line protocol
  //[key] [fields] [timestamp], The timestamp field (specified in nanoseconds from the epoch) is optional, so a sample point could look like this:
  //temperature,device=arduino01 value=83.2
  
  line = String("temperature value=" + temperature, "humidity value=" + humidity, "weight 1 value=" + weight_1, "weight 2 value=" + weight_2, "weight 3 value=" + weight_3, "weight 4 value=" + weight_4, "weight 5 value=" + weight_5, "weight 6 value=" + weight_6, "weight 7 value=" + weight_7, "weight_8 value=" + weight_8, "heater state=" + digitalRead(heater), "cooler state=" + digitalRead(cooler), "humidifier state=" + digitalRead(humidifier), "dehumidifier state=" + digitalRead(dehumidifier));
  Serial.println(line);
  
  // send the packet
  Serial.println("Sending UDP packet...");
  udp.beginPacket(host, port);
  udp.print(line);
  udp.endPacket();
  }
}
