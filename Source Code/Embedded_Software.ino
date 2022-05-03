/*
Used code found from these three sources to help with the project.
----------------------------------------------------------------
  US-100 Ultrasonic Sensor - Serial Mode
  modified on 26 Sep 2020
  by Mohammad Reza Akbari @ Electropeak
  Home
  Also used code from DIYTechBros to help with temperature readings
----------------------------------------------------------------
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
*/

// US100 range sensor include and define
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

SoftwareSerial US100Serial(5, 4);

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned int HighByte = 0;
unsigned int LowByte  = 0;
unsigned int Distance  = 0;
const float conv = 1000;
float DistanceCm = 0;
unsigned int Temp = 0;
float kg = 0;
float newtons = 0;
float stress = 0;
float area = 0;
float leng = 0;
float origLeng = 0;
float DistanceM;
int start = 0;
float deltaL = 0;
float strain = 0;
float initDistance = 0;

// Load sensor and HX711 include and define
#include <HX711.h>
#define calibration_factor -475000 // Calibration number found from calibration code
#define LOADCELL_DOUT_PIN  2
#define LOADCELL_SCK_PIN  3

// Load scale included
HX711 scale;

void setup() 
{
  Serial.begin(9600);
// Range test set up
  US100Serial.begin(9600);
  
// Load test set up
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); 
  scale.tare(); //reset the scale to 0

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);

  Serial.print("Enter cross sectional area: ");
  while(true) // remain here until told to break
  {
    
    if(Serial.available() > 0){ // did something come in?
      area = Serial.parseFloat();
      break;
    }
  }
  Serial.println(area);
  Serial.print("Enter original length of the sample: ");
  while(true) // remain here until told to break
  {
    
    if(origLeng == 0){ // did something come in?
      origLeng = Serial.parseFloat();
      delay(2000);
      if(origLeng != 0){
        break;
      }
    }
  }
  Serial.println(origLeng);
  Serial.println("Enter 1 when ready to start trial");
  while(true){
    if(start == 0){
      start = Serial.parseInt();
      delay(2000);
      if(start == 1){
        US100Serial.flush();
        US100Serial.write(0X55);                           // trig US-100 begin to measure the distance
        delay(150);                                  
          if (US100Serial.available() >= 2)                  // check receive 2 bytes correctly
          {
            HighByte = US100Serial.read();
            LowByte = US100Serial.read();
            Distance = HighByte * 256 + LowByte;            // Calculate the distance in mm
            initDistance = Distance / conv; 
            break;
          }
      }
    }
  }
  Serial.println(initDistance);
  delay(1000);
}

void loop() 
{
  //Range sensore loop start
  US100Serial.flush();
  US100Serial.write(0X55);                           // trig US-100 begin to measure the distance
  delay(100);                                  
  if (US100Serial.available() >= 2)                  // check receive 2 bytes correctly
  {
    HighByte = US100Serial.read();
    LowByte = US100Serial.read();
    Distance = HighByte * 256 + LowByte;            // Calculate the distance in mm
    DistanceCm = Distance / conv;                     // Calculate the distance in m
    if ((Distance > 1) && (Distance < 10000))
    {
      /*Serial.print("Distance: ");
      Serial.print(DistanceCm);               // Printing Distance in m to Serial monitor 
      Serial.println(" m"); */
      lcd.setCursor(0,1);
      lcd.print("DISTANCE: ");
      lcd.print(DistanceCm);                 
    }
  }

  US100Serial.flush();
  US100Serial.write(0X50);  // trig US-100 begin to measure the temperature    
  delay(100);                         
  if (US100Serial.available() >= 1)                // check receive 1 byte correctly
  {
    Temp = US100Serial.read();
    if((Temp > 1) && (Temp < 130))                // checks if temp is in range
    {
      Temp -= 45;                                 // Corrects the temperature 45 degree offset
      /*Serial.print("Temp: ");
      Serial.print(Temp, DEC);
      Serial.println(" ºC");*/
    }
  }                                              // Range sensor loop end
  
// calculating and printing load sensor loop
  /*Serial.print("Load: ");
  Serial.print(scale.get_units(), 1); //scale.get_units() returns a float
  Serial.println(" kg");*/
  kg = scale.get_units();
  newtons = kg * 9.81;
  /*Serial.print("Newtons: ");
  Serial.print(newtons);
  Serial.println(" N");*/
  lcd.setCursor(0,0);
  lcd.print("FORCE: ");
  lcd.print(newtons);
  stress = newtons / area;
  //Serial.print("Stress: ");
  Serial.print(stress); 
  Serial.print("\t");

  deltaL = (DistanceCm-initDistance);
  //Serial.print("Delta L: ");
  //Serial.println(deltaL);
  strain = deltaL / origLeng;
  //Serial.print("Strain: ");
  Serial.println(strain);
}
