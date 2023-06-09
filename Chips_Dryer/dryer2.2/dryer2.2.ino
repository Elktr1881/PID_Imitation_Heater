#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS  PA3
#define button1       PA8
#define button2       PA11
#define sensor        PA2 //KABEL HITAM
#define relay         PA1
#define buzzer        PB12

OneWire oneWire(ONE_WIRE_BUS);  
DallasTemperature sensors(&oneWire);

unsigned long previousMillis = 0;
unsigned long previousTime = 0;
unsigned long timeNow;
unsigned int count;

#define interval 45000  // Satuan ms(microseccond)
#define SetPoint 65.00  // Satuan Celcius

double temperature, start_temp, range_temp;
double timeNeed, tempNeed;
int suhu = 0, flag = 0, progres = 0; 

void setup(void)
{
  sensors.begin();
  Serial.begin(57600);

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(sensor, INPUT);
  pinMode(relay,  OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(PC13, OUTPUT);

  digitalWrite(buzzer, LOW);
}

double output, error;
double previous_error = 0;
double integral = 0;
double derivative = 0;

void PID(double tempNow, double Kp, double Ki, double Kd, double traceHold)
{
    bool result;
    unsigned long currentMillis = millis();  

//    Serial.print("  ");
//    Serial.print(currentMillis); Serial.print("  ");
//    Serial.print(previousMillis); Serial.print("  ");
//    Serial.print(start_temp); Serial.print("  ");
//    Serial.print(range_temp); Serial.print("  ");
//    Serial.print(timeNow); Serial.print("  ");
//    Serial.print(timeNeed); Serial.print("  ");
//    Serial.print(tempNow); Serial.print("  ");
//    Serial.print(tempNeed); Serial.print("  ");
    
    if(currentMillis - previousMillis >= interval) 
    {

      // Menghitung komponen PID
      integral = integral + (error * interval);
      derivative = (error - previous_error) / interval;
      output = (Kp * error) + (Ki * integral) + (Kd * derivative);
  
      // Menyimpan nilai error saat ini
      previous_error = error;

      // PID Convert time ON/OFF
      previousMillis = currentMillis;
      if(timeNow + Ki >= timeNeed - Kp){timeNow = timeNeed;}
      tempNeed = start_temp + (timeNow*range_temp/timeNeed);
      timeNow += (interval-(Kd*10));
    }

    //Aplication Output
    if(tempNow < tempNeed){digitalWrite(relay, HIGH);Serial.print("1"); Serial.print("@");} 
    else{digitalWrite(relay, LOW);Serial.print("0"); Serial.print("@");}   
}

void loop(void)
{  
  sensors.requestTemperatures();
  suhu = sensors.getTempCByIndex(0);
  temperature = sensors.getTempCByIndex(0);

  if(digitalRead(sensor) == HIGH and progres != 2){digitalWrite(PC13, LOW); progres = 2;}
  else{digitalWrite(PC13, HIGH);}
  
  if(digitalRead(button1) == LOW and progres != 1)
  {
    progres = 1; 
    timeNow = interval;
    start_temp = temperature;
    range_temp = SetPoint - start_temp;
    timeNeed = range_temp*interval;
  }
  else if(digitalRead(button2) == LOW and progres != 0){progres = 0;}

  Serial.print(suhu); Serial.print("#");
  switch(progres)
  {
    case 0:
      Serial.print("0"); Serial.print("@");
      Serial.print("B"); 
      digitalWrite(relay, LOW);
    break;
    
    case 1: 
      Serial.print("A"); 
      Serial.print(tempNeed); Serial.print("&");
      PID(temperature,5,2,0.8,SetPoint);
    break;
    
    case 2:
      Serial.print("C");Serial.print("0"); Serial.print("@");
      unsigned long currentTime = millis();
      if (currentTime - previousTime < 700) {digitalWrite(buzzer, HIGH);}
      else if (currentTime - previousTime < 1600) {digitalWrite(buzzer, LOW);}
      else{previousTime = currentTime;}
    break;
  }
  Serial.println();
}
