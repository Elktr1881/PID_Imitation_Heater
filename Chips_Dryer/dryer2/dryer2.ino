#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

#define CS_PIN PA4

File dataFile;
LiquidCrystal_I2C lcd(0x27,16,2);
RTClib myRTC;

String state[2] = {"OFF","HIT"};
String kode = "", str_inString = "", temp_kode = "";
int jam, menit, detik, tanggal, bulan, tahun;
int heat = 0, suhu = 0, set_point = 0, flag=0;

void setup(void) 
{
    Serial.begin(57600);
    Wire.begin();
    lcd.init();
    lcd.backlight();
  
    while (!SD.begin(CS_PIN))
    {
      lcd.setCursor(12,0);
      lcd.print("NOPE");
      delay(10);
    }
}

void tampil_data()
{
  DateTime now = myRTC.now();
  lcd.setCursor(0,1);
  lcd.print(String(jam)+":"+String(menit)+":"+String(detik)+" "+String(suhu)+"C "+state[heat]+"  ");
}
void getWaktu()
{
  DateTime now = myRTC.now();
  jam = now.hour();
  menit = now.minute();
  detik = now.second();
  tanggal = now.day();
  bulan = now.month();
  tahun = now.year();
}
void getData(String text)
{
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if(dataFile) 
  {
      dataFile.print(text);
      dataFile.print(" : ");
      dataFile.print(String(jam)+":"+String(menit)+":"+String(detik));
      dataFile.print("  ");
      dataFile.print(String(tanggal)+":"+String(bulan)+":"+String(tahun));
      dataFile.print(" -> suhu: ");
      dataFile.println(suhu);
      dataFile.close();
      
      lcd.setCursor(0,0);
      lcd.print("Saving Data...     ");
      lcd.setCursor(0,1);
      lcd.print("   Please Wait...  ");
  }
  else {Serial.println("error opening datalog.txt");lcd.setCursor(12,0); lcd.print("FAIL");}

  delay(1500);
  lcd.clear();
}

void GetSerial()
{
    while(Serial.available())
    {
        int inchar = Serial.read();
        if(isDigit(inchar)){str_inString+=(char)inchar;} 
        else if(isAlpha(inchar)){kode = (char)inchar;}

        if(inchar == '&') {str_inString= "";}
        if(inchar == '.') 
        { 
          set_point = str_inString.toInt();;
          str_inString= "";
        }
        
        if(inchar == '#')
        {
          suhu = str_inString.toInt();;
          str_inString= "";
        }
        if(inchar == '@')
        {
          heat = str_inString.toInt();;
          str_inString= "";
        }
    }
}

void loop(void) 
{
    GetSerial();
    getWaktu();

    if(temp_kode != kode){lcd.clear(); flag=0;}
    temp_kode = kode;
    
    if(kode == "A")
    {
      if(flag == 0){getData("Machine ON");}
      lcd.setCursor(1,0);
      lcd.print("Running Machine");
      tampil_data();
      flag=1;
    }
    else if(kode == "B")
    {
      if(flag == 0){getData("Machine OFF");}
      lcd.setCursor(1,0);
      lcd.print("Teknik Elektro");
      tampil_data();    
      flag=1;
    } 
    else if(kode == "C")
    {
      if(flag == 0){getData("Warning Metal Detection");}
      lcd.setCursor(0,0);
      lcd.print("Metal Detection"); 
      lcd.setCursor(7,1);
      lcd.print("!!!");
      flag=1;
    }      
}
