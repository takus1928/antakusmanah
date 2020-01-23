#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

LiquidCrystal_I2C lcd(0x27,16,2);
RTC_DS3231 rtc;

const byte ROWS = 4 ;
const byte COLS = 4 ;
 
char keys [ROWS][COLS] =
{{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}};

byte rowPins [ROWS] = {13,12,11,10};
byte colPins [COLS] = {9,8,7,6};

int i =0;

Keypad myKeypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);

int relay1 = 5;
int relay2 = 4;
int buzzer = 3;

char customKey , customKeyx;
int i1,i2,i3,i4;
char c1,c2,c3,c4;

int jamA = NULL;
int menitA = NULL;
int aturWaktuA = NULL;





void setup()
{
  Serial.begin(9600);
	lcd.init();
	lcd.backlight();
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(buzzer, OUTPUT);
  deteksiRTC();
  
}

void loop()
{
  while (customKey == NO_KEY){
    
    customKey = myKeypad.getKey();
    Serial.println(customKey);
    
    DateTime now = rtc.now();

    if(now.hour() == jamA && now.minute() == menitA && aturWaktuA == 1 && now.second() >= 0 && now.second() <= 2 ){
     while(customKeyx == NO_KEY){
     tone(buzzer, 1000); //You can modify the tone or make your own sound
     delay(100);
     tone(buzzer, 2000);
     delay(100);     
     digitalWrite( relay1, HIGH);
     customKeyx = myKeypad.getKey();
      }
    }
    customKeyx = NO_KEY;
    noTone(buzzer); 
    digitalWrite( relay1, LOW);
    waktu();
    delay(1000);
    /*lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PENGERING  PAKAN");
    lcd.setCursor(0,1);
    lcd.print("A : Atur Waktu");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PENGERING  PAKAN");
    lcd.setCursor(0,1);
    lcd.print("B : Reset Waktu");
    delay(2000);
    */
  }

  if (customKey == 'A'){

    lcd.clear();
    lcd.setCursor(0 , 0);
    lcd.print("  Atur Waktu  ");
    lcd.setCursor(0 , 1);
    lcd.print("Jam : ");

    char customKey1 = myKeypad.waitForKey();
                    if (customKey1 != NO_KEY && customKey1 !='*' && customKey1 !='#' && customKey1 !='A' && customKey1 !='B' && customKey1 !='C' && customKey1 !='D' )
                      {
                       c1 = customKey1;
                       lcd.setCursor(7, 1);
                       lcd.print(c1);
                       }
 
    char customKey2 = myKeypad.waitForKey();
                    if (customKey2 != NO_KEY && customKey2 !='*' && customKey2 !='#' && customKey2 !='A' && customKey2 !='B' && customKey2 !='C' && customKey2 !='D' )
                      {
                       c2 = customKey2;
                       lcd.setCursor(8, 1);
                       lcd.print(c2);
                       }
                       
                     i1=(c1-48)*10;
                     Serial.println(i1);
                     i2=c2-48;
                     Serial.println(i2);
                     jamA = i1+i2;

                    if (jamA >= 25){
                      lcd.setCursor(5, 1);
                      lcd.print("Atur Ulang");
                      delay(2000);
                      return jamA;
                    }
                    delay(1000);
                    
                    lcd.clear(); 
                    lcd.clear();
    lcd.setCursor(0 , 0);
    lcd.print("  Atur Waktu  ");
    lcd.setCursor(0 , 1);
    lcd.print("Menit : ");

    char customKey3 = myKeypad.waitForKey();
                    if (customKey3 != NO_KEY && customKey3 !='*' && customKey3 !='#' && customKey3 !='A' && customKey3 !='B' && customKey3 !='C' && customKey3 !='D' )
                      {
                       c3 = customKey3;
                       lcd.setCursor(7, 1);
                       lcd.print(c3);
                       }
 
    char customKey4 = myKeypad.waitForKey();
                    if (customKey4 != NO_KEY && customKey4 !='*' && customKey4 !='#' && customKey4 !='A' && customKey4 !='B' && customKey2 !='C' && customKey2 !='D' )
                      {
                       c4 = customKey4;
                       lcd.setCursor(8, 1);
                       lcd.print(c4);
                       }
                       
                     i3=(c3-48)*10;
                     Serial.println(i3);
                     i4=c4-48;
                     Serial.println(i4);
                     menitA = i3+i4;

                    if (menitA >= 61){
                      lcd.setCursor(5, 1);
                      lcd.print("Atur Ulang");
                      delay(2000);
                      return menitA;
                    }
                    delay(1000);
                    lcd.clear();
                    aturWaktuA = 1;
                    customKey = NO_KEY;
               }
    
    if (customKey == 'B'){
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
                    
                    lcd.clear();
                    lcd.setCursor(2,0);
                    lcd.print("Reset Waktu");
                    delay(2000);
                    aturWaktuA = 0;
                    customKey = NO_KEY;
    }
    
    if (customKey == 'C'){
                    
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("Batal Atur Waktu");
                    delay(2000);
                    aturWaktuA = 0;
                    customKey = NO_KEY;
    }
}

 void deteksiRTC(){
  
  if ( !rtc.begin() ) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

 if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

 void waktu(){
  DateTime now = rtc.now();
   
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print(now.day(), DEC);
    lcd.print('-');
    lcd.print(now.month(), DEC);
    lcd.print('-');
    lcd.print(now.year(), DEC);
    lcd.print("  ");
    lcd.setCursor(4,1);
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);    
   // delay(1000);
  
 }
