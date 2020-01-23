#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "shfs";
const char *password = "shfs1234";

//Web/Server address to read/write from
const char *host = "http://shfs.tekom.id/";
/*contoh "https://wind.speed.id/" */

SoftwareSerial s(D3,D4);
//========================= Inisialisasi SD-CARD =====================
File dataFile;
const int CS = D8;
char filename[16];
String fileName;

void setup() {

//========================== COMMUNICATION NODEMCU ====================================
  Serial.begin(9600);
  s.begin(9600);
  while (!Serial) continue;

//========================== FUNGSI =============================  
  pinMode(CS, OUTPUT);
  konekWifi();
  inisSdCard(); 
}

void loop() {

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid())
    return;

  Serial.println("JSON received and parsed");
  root.prettyPrintTo(s);
  Serial.print("   Tanggal ");
  Serial.print("");
  String tanggal = root["data5"];
  Serial.print(tanggal);
  Serial.println("");
  Serial.print("  Waktu ");
  String waktu = root["data4"];
  Serial.print(waktu);
  Serial.println("");
  Serial.print("   Suhu ");
  Serial.println("");
  String suhu = root["data2"];
  Serial.print("   Kelembaban");
  String kelembaban = root["data1"];
  Serial.print(kelembaban);
  Serial.print("   Kecepatan Angin");
  String  speedwind = root["data3"];
  Serial.print(speedwind);
  Serial.println("");
  
  Serial.println("---------------------xxxxx--------------------");
 
 
  char charFileName[fileName.length() + 1];
  fileName.toCharArray(charFileName, sizeof(charFileName));

  dataFile = SD.open(charFileName, FILE_WRITE);
  if(dataFile){
    dataFile.print(speedwind);
    dataFile.print(" |  ");
    dataFile.print(suhu);
    dataFile.print("  |  ");    
    dataFile.print(kelembaban);
    dataFile.print("  |  ");    
    dataFile.print(tanggal);
    dataFile.print("   |  ");    
    dataFile.println(waktu);
    dataFile.close();  
  } else {
    Serial.println("GAGAL TULIS DATA KE SD CARD"); 
  } 

  HTTPClient http;
  String postData;

  Serial.println(tanggal);
  Serial.println(waktu);
  Serial.println(suhu);
  Serial.println(kelembaban);
  Serial.println(speedwind);
  //Post Data
  postData = "&suhu=" + suhu + "&kelembapan=" + kelembaban + "&kecepatanangin=" + speedwind;

  http.begin("http://shfs.tekom.id/api.php");       //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header

  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload

  http.end();  //Close connection
  delay(1000);
 
 delay(2000);
}

//========================== SD CARD =================================
void inisSdCard(){
  Serial.print("Initializing SD card...");
  
  if (SD.begin(CS))
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
  Serial.println("initialization done.");

  fileName = String();
  String message = String();
  unsigned int filenumber = 1;
  while(!filenumber==0) {
    fileName = "data_";
    fileName += filenumber;
    fileName += ".txt";
    message = fileName;
    char charFileName[fileName.length() + 1];
    fileName.toCharArray(charFileName, sizeof(charFileName));

    if (SD.exists(charFileName)) { 
      message += " exists.";
      filenumber++;
    }
    else {
      Serial.println(charFileName);
      dataFile = SD.open(charFileName,FILE_WRITE);
      if(dataFile){
        dataFile.println("Speed(m/s) | Suhu Udara(C) | Lembab Udara(%) |   Date    |   Time   ");
        dataFile.close();  
        Serial.println("Writing Data To Sdcard...");
      } else {
        Serial.println("GAGAL TULIS NAMA KOLOM SD CARD");
      }
      filenumber = 0;   
    }
  }
  
  Serial.println("done.");
  Serial.println(" ");
}


void konekWifi(){
  
  delay(1000);
  Serial.begin(9600);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  Serial.println("");
  int x = 0;
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
      if (x >= 20){
      break;
    }
    x++;
    Serial.print(x);
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  x = 0;
  }
