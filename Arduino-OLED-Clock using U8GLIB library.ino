/******************************************************************************
OLEd Analog Clock using U8GLIB Library

visit https://code.google.com/p/u8glib/ for full details of the U8GLIB library and
full instructions for use.

by Chris Rouse Oct 2014

Note:: Sketch uses 53% of program storage space, 
       Global variables use 48% of dyamic memory, 
       leaving 1,061 bytes for local variables.

Using a IIC 128x64 OLED with SSD1306 chip
RTC DS1307 
Optional Temperature Sensor TMP 36

Wire RTC:
  VCC +5v
  GND GND
  SDA Analog pin 4
  SCL Analog pin 5
Wire OLED:
  VCC +5v
  GND GND
  SDA Analog pin 4
  SCL Analog pin 5
Wire TMP36:
  VCC +3.3v
  GND GND
  Out Analog pin 1
  
Connect ARef to 3.3v on Arduino

******************************************************************************/

// Add libraries
  #include "U8glib.h"
  #include <SPI.h>
  #include <Wire.h>
  #include "RTClib.h"
  
// setup u8g object
  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C 
//

// Setup RTC
  RTC_DS1307 RTC;
  char monthString[37]= {"JanFebMarAprMayJunJulAugSepOctNovDec"};
  int  monthIndex[122] ={0,3,6,9,12,15,18,21,24,27,30,33};
  String thisMonth = "";
  String thisTime = "";
  String thisDay="";
  
//
//TMP36 Pin Variables
  boolean useTMP36 = true;  // set this to false if you don not use TMP36
//boolean useTMP36 = false;
  #define aref_voltage 3.3 // we tie 3.3V to ARef and measure it with a multimeter!
  int tempPin = 1; //the analog pin the TMP36's Vout (sense) pin is connected to
//the resolution is 10 mV / degree centigrade with a
//500 mV offset to allow for negative temperatures
  int tempReading; // the analog reading from the sensor
//
 

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  //u8g.setFont(u8g_font_5x8);
  u8g.setFont(u8g_font_6x10); 
  //
  //***** RTC **********
  DateTime now = RTC.now();
  // display date at bottom of screen
  thisDay = String(now.day(), DEC) + "/"; 
  thisMonth="";
  for (int i=0; i<=2; i++){
    thisMonth += monthString[monthIndex[now.month()-1]+i];
  }  
  thisDay=thisDay + thisMonth + "/"; 
  thisDay=thisDay + String(now.year() , DEC);
  const char* newDay = (const char*) thisDay.c_str(); 
  u8g.drawStr(32,63, newDay);   
  // ********************* 
  // display time in digital format
  thisTime="";
  thisTime=String(now.hour()) + ":";
  if (now.minute() < 10){ thisTime=thisTime + "0";} // add leading zero if required
  thisTime=thisTime + String(now.minute()) + ":";
  if (now.second() < 10){ thisTime=thisTime + "0";} // add leading zero if required
  thisTime=thisTime + String(now.second());
  const char* newTime = (const char*) thisTime.c_str();
  u8g.drawStr(10,10, newTime);  
  // ********************* 
  //
  // Now draw the clock face
  u8g.drawCircle(64, 32, 20);
  u8g.drawCircle(64, 32, 2);
  //
  //hour ticks
  for( int z=0; z < 360;z= z + 30 ){
  //Begin at 0° and stop at 360°
    float angle = z ;
    angle=(angle/57.29577951) ; //Convert degrees to radians
    int x2=(64+(sin(angle)*20));
    int y2=(32-(cos(angle)*20));
    int x3=(64+(sin(angle)*(20-5)));
    int y3=(32-(cos(angle)*(20-5)));
    u8g.drawLine(x2,y2,x3,y3);
  }
  // display second hand
  float angle = now.second()*6 ;
  angle=(angle/57.29577951) ; //Convert degrees to radians  
  int x3=(64+(sin(angle)*(20)));
  int y3=(32-(cos(angle)*(20)));
  u8g.drawLine(64,32,x3,y3);
  //
  // display minute hand
  angle = now.minute() * 6 ;
  angle=(angle/57.29577951) ; //Convert degrees to radians  
  x3=(64+(sin(angle)*(20-3)));
  y3=(32-(cos(angle)*(20-3)));
  u8g.drawLine(64,32,x3,y3);
  //
  // display hour hand
  angle = now.hour() * 30 + int((now.minute() / 12) * 6 )   ;
  angle=(angle/57.29577951) ; //Convert degrees to radians  
  x3=(64+(sin(angle)*(20-11)));
  y3=(32-(cos(angle)*(20-11)));
  u8g.drawLine(64,32,x3,y3);
 //
 // now add temperature if needed
 if (useTMP36==true) {
 //getting the voltage reading from the temperature sensor
   tempReading = analogRead(tempPin);   
 // converting that reading to voltage, for 3.3v arduino use 3.3
   float voltage = tempReading * aref_voltage;
   voltage /= 1024.0; 
 // now print out the temperature
   int temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
   String thisTemp1 = String(temperatureC) + "C";
   const char* thisTemp = (const char*) thisTemp1.c_str();
   u8g.drawStr(100,10,thisTemp); 
 }
//
//
}

void setup(void) {
  Serial.begin(9600);
  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

}

void loop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(50);
}
