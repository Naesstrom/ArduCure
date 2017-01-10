#ifndef READ_DHT_H
#define READ_DHT_H
#include <Arduino.h>;

void readDHT(){
  humidity=0;
  temperature=0;
  int reads=0;
  int whileCount=0;
  //averaging three readings, for better accuracy
  while(reads<3){
    whileCount++;
//    delay(dht.getMinimumSamplingPeriod()+10);
    //evaluating sensor status
    dhtStatus=dht.getStatus();
    dhtStatusString=dht.getStatusString();
    //if status is ok sensor is read.
    if(!DHT.getStatus()){
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

#endif //__READ_DHT_H
