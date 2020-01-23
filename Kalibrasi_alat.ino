#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include "HX711.h"

#define DOUT A1
#define CLK  A0

//============== LCD 20x4 ============================
LiquidCrystal_I2C lcd(0x27, 20, 4);

//============== RTC DS3231 ==========================
RTC_DS3231 rtc;

//============= SD CARD ==============================

File myFile;
const int CS = 53;

//======================== SENSOR HUJAN ================
int pin_do = 4;
int sensor_hujan;

//========================= CURAH HUJAN ==========================
float luas_lingkaran = 0.0961625; //satuan m2
float berat;
float berat_p;
float berat_akhir;
float bas = 0 ;
float curah_hujan;
float berat_total;
float curah_hujan_tot;
String curah_hujan1;
String curah_hujan2;
String awal_hujan;
String akhir_hujan;
int jumlah_data = 0;
int jumlah_data1 = 0;
String waktu;
String tanggal;
String tanggal_server;
int temp_berat = 0;
int x = 0 ; 
int stabil = 0;
boolean a = false;
float beratak = 0;

//============== LOAD CELL + HX711 =================================
HX711 skala(DOUT, CLK);
float faktor_kalibrasi = 692.367;

//============== ESP 8266 - 01 =====================================
String ssidWifi = "RedmiTEK";
String passWifi = "tekkom9888";
String IP = "curahhujan.pesan.website";

boolean connected=false;

long waktuKirim = 17; 
long waktuMulai = 0;
long waktuTunggu = 0;

boolean error;

//===================================================================

void setup() {
//========================== PIN SENSOR HUJAN ========================
  pinMode(pin_do , INPUT);

//========================== SERIAL MONITOR ==========================
  Serial.begin(9600);
  Serial1.begin(115200);
  
//========================== LCD 16x2 =============================  
  lcd.init();
  lcd.backlight();
  
  //deteksi_rtc();
  tampilan_awal_lcd();  
      
      lcd.setCursor(3,2);
      lcd.print("PLEASE WAIT...");
      
 // inis_sd_card();
  //konek_wifi();
  tampilan_awal_lcd();

      lcd.setCursor(2,2);
      lcd.print("WIFI CONNECTED..");
      delay(2000);

//========================== LOADCELL+HX711===========================
  skala.set_scale();
  skala.tare();
  skala.set_scale(faktor_kalibrasi);
  
}

void loop() {
  
  sensor_hujan = digitalRead(pin_do);

  float berat_total1 = 0;
    
    if (sensor_hujan == LOW)
            { 
    //              buat_file_H();
      //            cari_waktu();
                  a = true;
        //          awal_hujan = waktu;
                     
            }   
          else
            { 
              DateTime now = rtc.now();
                  tampilan_awal_lcd();
                  lcd.setCursor(4, 2);
                  lcd.print("TIDAK HUJAN");
                  delay(1000);
                  
            }
                 while (a){
                       x++;         
                   if (x > 10){
                         beratak = berat_total1/jumlah_data1;
                         monitoring_lcd_histori2();
                         Serial.println (berat_total1);
                         Serial.println (jumlah_data1);
                         delay(2000);
                         }
                   else
                   {
                    Serial.println(x);
                    cari_berat();
                        berat_total1 = berat_total1 + berat;
                        jumlah_data1 = jumlah_data1 + 1 ;
                        tampilan_awal_lcd();
                        monitoring_lcd_histori();
                        //kirim_sdcard_histori();
                        delay(1000);
                        akhir_hujan = waktu;
                   }

                                int sensor_hujan1 = digitalRead(pin_do);

                                    if(sensor_hujan1 == HIGH)
                                      {  
                                        a = false;
                                        cari_waktu();
                                        berat_total = berat_total1;
                                        jumlah_data = jumlah_data1;
                                        cari_curah_hujanT();
                                        
                                            tampilan_awal_lcd();
                                            lcd.setCursor(4, 2);
                                            lcd.print("HUJAN SELESAI");
                                            delay(2000);
                                            
                                        /*monitoring_lcd_total();
                                        delay(2000);
                                        
                                        tampilan_awal_lcd();
                                        lcd.setCursor(4, 2);
                                        lcd.print("MENGIRIM DATA..");
                                        //kirim_sdcard_ha();
                                        //kirim_sdcard_total();*/
                                        x = 0;
                                        stabil = 0;
                                        bas = 0;
                                        jumlah_data1 = 0;
                                        berat_total1 = 0;
                                        beratak = 0;
                                        //kirim_server();
                                        delay(1000);
                                    }
                 }

}
//====================================================================
void cari_berat(){
    berat = skala.get_units(), 4;
   // berat_p = 1.461+(6.951*berat)+(-0.068*(berat*berat))+(0.00026*(berat*berat*berat))+(-0.000000034*(berat*berat*berat*berat));
   // Serial.print (" BERAT AKHIR SEBELUM1  : "); Serial.print (bas); Serial.println ("   ");   
    Serial.print (" BERAT  : "); Serial.print (berat); Serial.println ("   "); 
    //berat_akhir = berat_p - bas;
    //Serial.print (" BERAT AKHIR  : "); Serial.print (berat_akhir); Serial.println ("   ");
    //if (berat_akhir < 0)
    //  berat_akhir = abs(berat_akhir);
    curah_hujan = berat_akhir / luas_lingkaran;
    //bas = berat_p;
    //Serial.print (" BERAT AKHIR SEBELUM2  : "); Serial.print (bas); Serial.println ("   ");
    
    char str[50];
      strcpy(str, "");
        dtostrf(curah_hujan, 2, 6, &str[strlen(str)]);
        curah_hujan1 = str;
        
}

void cari_curah_hujanT(){
  curah_hujan_tot = (berat_total*0.001) / luas_lingkaran;
    char str[50];
      strcpy(str, "");
        dtostrf(curah_hujan_tot, 2, 6, &str[strlen(str)]);
        curah_hujan2 = str;
}

//========================== SD CARD =================================
void inis_sd_card(){
      
      Serial.print("Initializing SD card...");
      
      if (!SD.begin(CS)) {
        Serial.println("Card failed, or not present");
        return;
      }
      Serial.println("initialization done.");

      
      if(!SD.exists("datach.txt"))
          {
          File dataFile = SD.open("datach.txt",FILE_WRITE);
          dataFile.println(" Tanggal   Jam Awal Hujan  Jam Akhir Hujan    Volume Air(mL)  Curah Hujan(mm)");
          dataFile.close();
          }
      else
          {
           Serial.println("Datach.txt Sudah Tersedia");
          }
      
}

      void buat_file_H(){
          
        File dataFile = SD.open("histori.txt", FILE_WRITE);
         if (dataFile) {
              dataFile.println(" ");
              dataFile.println("  NO     TANGGAL      WAKTU       DATA   ");  
              dataFile.close();
            }
        }

        
        void kirim_sdcard_histori(){
        
          cari_waktu();
          cari_tanggal();
        
        File dataFile = SD.open("histori.txt", FILE_WRITE);
        
          if (dataFile) {
              dataFile.print("  ");
              dataFile.print(jumlah_data1);
              dataFile.print("    ");
              dataFile.print(tanggal);
              dataFile.print("    ");
              dataFile.print(waktu);
              dataFile.print("     ");
              dataFile.print(berat_akhir);
              dataFile.println("  ");
              dataFile.close();
            }
            else
            {
              Serial.println("DATA TIDAK MASUK KE SD CARD");
            }
        }

        void kirim_sdcard_ha(){
        
        File dataFile = SD.open("histori.txt", FILE_WRITE);
        
          if (dataFile) {
              dataFile.println("  ");
              dataFile.println("================================================================");
              dataFile.print(" Jumlah Pengukuran : ");
              dataFile.println(jumlah_data);
              dataFile.print(" Total Data : ");
              dataFile.println(berat_total);
              dataFile.print(" Mulai Hujan : ");
              dataFile.println(awal_hujan);
              dataFile.print(" Berhenti Hujan : ");
              dataFile.println(akhir_hujan);
              dataFile.println("  ");
              dataFile.println("================================================================");
              dataFile.println("  ");
              dataFile.println("  ");
              dataFile.close();
            }
            else
            {
              Serial.println("DATA TIDAK MASUK KE SD CARD");
            }
        }
        
        void kirim_sdcard_total(){
          
        cari_tanggal();
        
        File dataFile = SD.open("datach.txt", FILE_WRITE);
        
          if (dataFile) {
              
              dataFile.print("  ");
              dataFile.print(tanggal);
              dataFile.print("        ");
              dataFile.print(awal_hujan);
              dataFile.print("        ");
              dataFile.print(akhir_hujan);
              dataFile.print("         ");
              dataFile.print(berat_total);
              dataFile.print("         ");
              dataFile.print(curah_hujan2);
              dataFile.println("     ");
              dataFile.close();
            }
            else
            {
              Serial.println("DATA TIDAK MASUK KE SD CARD");
            }
        }

//====================== RTC DS3231 ============================
void deteksi_rtc(){
  
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }
    
    //rtc.begin(DateTime(__DATE__, __TIME__));
  
   if (rtc.lostPower()) {
      Serial.println("RTC lost power, lets set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }
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

//========================== TAMPILAN AWAL LCD =======================
void tampilan_awal_lcd(){

            cari_tanggal();
            cari_waktu();
            lcd.clear();
            lcd.setCursor(1 ,0);
            lcd.print("-Rainfall  Sensor-");
            lcd.setCursor(0 ,1);
            lcd.print(tanggal);
            lcd.setCursor(12 ,1);
            lcd.print(waktu);
}

//======================== LAYAR MONITORING ==========================
void monitoring_lcd_histori(){

            lcd.clear();
            tampilan_awal_lcd();
            lcd.setCursor(1, 2);
            lcd.print("V : ");
            lcd.setCursor(7, 2);
            lcd.print(berat);
            lcd.setCursor(17, 2);
            lcd.print("mL");
        
        }

void monitoring_lcd_histori2(){

            lcd.clear();
            tampilan_awal_lcd();
            lcd.setCursor(1, 2);
            lcd.print("V : ");
            lcd.setCursor(7, 2);
            lcd.print(beratak);
            lcd.setCursor(17, 2);
            lcd.print("mL");
        
        }
        
void monitoring_lcd_total(){
        
            lcd.clear();
            tampilan_awal_lcd();
            lcd.setCursor(1, 2);
            lcd.print("V : ");
            lcd.setCursor(7, 2);
            lcd.print(berat_total);
            lcd.setCursor(17, 2);
            lcd.print("mL");
        
            lcd.setCursor(1, 3);
            lcd.print("C : ");
            lcd.setCursor(7, 3);
            lcd.print(curah_hujan2);
            lcd.setCursor(17, 3);
            lcd.print("mm");
        }

//========================== KONEKSI KE  WIFI ========================
void konek_wifi(){
  
  unsigned char cekKoneksi=0;
  unsigned char waktuCek=0;
  
  waktuMulai = millis();
    
    Serial1.println("AT+RST");
    delay(1750);

    Serial.println("Menghubungkan Ke WIFI");
    while(cekKoneksi==0){
        Serial.print(".");
        Serial1.print("AT+CWJAP=\""+ssidWifi+"\",\""+passWifi+"\"\r\n");
        Serial1.setTimeout(5000);
             if(Serial1.find("WIFI CONNECTED\r\n")==1)
                   {
                   Serial.println("WIFI CONNECTED");
                   break;
                   }
                   waktuCek++;
                      if(waktuCek>3) 
                           {
                            waktuCek=0;
                            Serial.println("Trying to Reconnect..");
                           }            
    }
}

void kirim_server()
{
     
      cari_tanggal();
      startCmd();
      
      String getStr = "GET /update.php?";
      //getStr += myAPIkey;
      
      getStr +="tanggal=";
      getStr += tanggal_server;
      getStr +="&waktu_awal=";
      getStr += awal_hujan;
      getStr +="&waktu_akhir=";
      getStr += akhir_hujan; 
      getStr +="&volume_air=";
      getStr += String(berat_total);
      getStr +="&curah_hujan=";
      getStr += String(curah_hujan2);
      getStr += " HTTP/1.1\r\nHost:" + IP;
      getStr += "\r\n\r\n";
      getCmd(getStr); 
      //localhost:8080/rainfall-kon/update.php?tanggal=2019-10-11&waktu=12:00:00&waktu_awal=12:00:00&waktu_akhir=12:00:00&volume_air=12.00&curah_hujan=0.0012
     
}

void startCmd()
{
      Serial1.flush();
      String cmd = "AT+CIPSTART=\"TCP\",\"";
      cmd += IP ; //IP Server, IP Website, atau IP PC
      cmd += "\",80";
      Serial1.println(cmd);
      Serial.print("Start Commands: ");
      Serial.println(cmd);

  if(Serial1.find("Error"))
      {
        Serial.println("AT+CIPSTART error");
        return;
      }
}

String getCmd(String getStr)
{
    String cmd = "AT+CIPSEND=";
    cmd += String(getStr.length());
    Serial1.println(cmd);
    Serial.println(cmd);

  if(Serial1.find(">"))
      {
        Serial1.print(getStr);
        Serial.println(getStr);
        delay(1000);
        String messageBody = "";
        while (Serial1.available()) 
        {
          String line = Serial1.readStringUntil('\n');
          if (line.length() == 1) 
          { 
            messageBody = Serial1.readStringUntil('\n');
          }
        }
        Serial.print("MessageBody received: ");
        Serial.println(messageBody);
        return messageBody;
      }
      
      else
      
      {
       //Serial1.println("AT+CIPCLOSE");     
       Serial.println("AT+CIPCLOSE"); 
      } 
      Serial.print("GAGAL UPLOAD");
}
//====================================================================================
