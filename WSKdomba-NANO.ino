#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <DHT.h>
//#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"




//========================= Inisialisasi Serial Communication NodeMCU =====================
SoftwareSerial s(5,6);
//========================= Inisilisasi RTC DS3231 ===================
RTC_DS3231 rtc;

//========================= Inisialisasi Sensor Suhu | DHT22 =====================
#define DHTPIN 3
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE); 
float suhu;
//float kelembaban1;
float kelembaban;

//======================== Inisialisasi Sensor Kecepatan Angin =================
# define windPin 2

const float pi = 3.14159265; // pi number
int period = 10000; // Measurement period (miliseconds)
int delaytime = 10000; // Time between samples (miliseconds)
int radio = 80; // Distance from center windmill to outer cup (mm)
int jml_celah = 18; // jumlah celah sensor

unsigned int counter = 0; // B/W counter for sensor
unsigned int RPM = 0; // Revolutions per minute
float speedwind = 0; // Wind speed (m/s)
//float speedwind = 0;
String tanggal;
String waktu;
String tanggal_server;

void setup(){

  s.begin(9600);
//========================== RTC =====================================
  rtc.begin();
  dht.begin();
//========================== COMMUNICATION NODEMCU =====================================
  
  Serial.begin(9600);

//========================== FUNGSI =============================   
  deteksi_rtc();

}

void loop() {
  cari_tanggal();
  cari_waktu();
  Serial.print(tanggal);
  Serial.print("===");
  Serial.print(waktu);
  Serial.println("===");
  sensor_KecepatanAngin();
  sensor_DHT();
  kirimServer();
  delay(10000);
}

//============================= KIRIM SERVER =========================
void kirimServer(){
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["tanggal"] = tanggal_server;
  root["waktu"] = waktu;
  root["suhu_udara"] = suhu;
  root["kelembaban_udara"] = kelembaban;
  root["speedwind"] = speedwind;
  //if(s.available()>0)
  //{
   root.printTo(s);
  //}

}

//========================== Sensor Suhu | DHT22 =============================  
void sensor_DHT(){
  
  
  kelembaban = dht.readHumidity();
  //kelembaban = ( 1.2929 * kelembaban1 ) - 22.679;
  suhu = dht.readTemperature();

 /* if (isnan(kelembaban) || isnan(suhu)) {
    digitalWrite(41, HIGH);
    digitalWrite(40, LOW);
    Serial.println("Error in reading sensor data!");
    while (isnan(kelembaban) || isnan(suhu)) {
      kelembaban = dht.readHumidity();
    //  kelembaban = ( 1.2929 * kelembaban1 ) - 22.679;
      suhu = dht.readTemperature();
    }
  }
  else {
    digitalWrite(41, LOW);
    digitalWrite(40, HIGH);
  }*/
  
  Serial.print("Kelembaban: ");
  Serial.print(kelembaban);
  
  //Serial.print("Kelembaban 1: ");
  //Serial.print(kelembaban1 );
  Serial.print(" %, Suhu: ");
  Serial.print(suhu);
  Serial.println(" Celsius");
}


//======================== Sensor Kecepatan Angin =============================
void sensor_KecepatanAngin(){
  Serial.print("Start measurement");
  windvelocity();
  Serial.println(" finished.");
  Serial.print("Counter: ");
  Serial.print(counter);
  Serial.print("; RPM: ");
  RPMcalc();
  Serial.print(RPM);
  Serial.print("; Wind speed: ");
  WindSpeed();
  Serial.print(speedwind);
  Serial.print("[m/s]");
  Serial.println("");
/*
  if (isnan(speedwind)) {
    digitalWrite(39, HIGH);
    digitalWrite(38, LOW);
    Serial.println("Error in reading sensor data!");
    while (isnan(speedwind)) {
      windvelocity();   
      RPMcalc(); 
      WindSpeed();  
    }
  }
  else {
    digitalWrite(39, LOW);
    digitalWrite(38, HIGH);
  }
  */
}

void windvelocity(){
  speedwind = 0;
  counter = 0;
  attachInterrupt(0, addcount, CHANGE);
  unsigned long millis();
  long startTime = millis();
  while(millis() < startTime + period) {}
  detachInterrupt(1);
}

void RPMcalc(){
  RPM=((counter/jml_celah)*60)/(period/1000); // Calculate revolutions per minute (RPM)
}

void WindSpeed(){
  speedwind = ((2 * pi * radio * RPM)/60) / 1000; // Calculate wind speed on m/s
  //speedwind = ( 1.1235 * kspeedwind ) + 0.6317;
}

void addcount(){
  counter++;
}



//====================== RTC DS3231 ============================
void deteksi_rtc(){
  
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }
    
    //rtc.begin(DateTime(__DATE__, __TIME__));
  
  /*if (rtc.lostPower()) {
      Serial.println("RTC lost power, lets set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }*/
}

//======================== CARI TANGGAL dan WAKTU ====================

      void cari_tanggal(){
        
        DateTime now = rtc.now();
        
        char ctanggal[20];  
       
          sprintf(ctanggal, "%d-%d-%d",     
                now.day(),   
                now.month(),  
                now.year()    
               );
           tanggal = ctanggal;
         
         char ctanggals[20];  
       
          sprintf(ctanggals, "%d-%d-%d",     
                now.year(),   
                now.month(),  
                now.day()    
               );
           tanggal_server = ctanggals;
      }
      
      void cari_waktu (){
          
          DateTime now = rtc.now();
         
          char cwaktu[20];   
       
          sprintf(cwaktu, "%d:%d:%d",         
                now.hour(),   
                now.minute(), 
                now.second()  
               );
               
           waktu = cwaktu;
      }
