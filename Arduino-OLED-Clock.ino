/***************************************************************
OLED Analogue Clock using Adafruit GFX and OLED libraries

by Chris Rouse Oct 2014

Using a IIC 128x64 OLED with SSD1306 chip
RTC DS1307 
Temperature probe TMP 36

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
***************************************************************/
// Add libraries
  #include <SPI.h>
  #include <Wire.h>
  #include "RTClib.h"
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
//
// setup OLED display
  #define OLED_RESET 4
  Adafruit_SSD1306 display(OLED_RESET);
  #if (SSD1306_LCDHEIGHT != 64)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
  #endif
//
// Setup RTC
  RTC_DS1307 RTC;
  char monthString[37]= {"JanFebMarAprMayJunJulAugSepOctNovDec"};
  int  monthIndex[122] ={0,3,6,9,12,15,18,21,24,27,30,33};
  char monthName[3]="";
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

void setup() {
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
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  // Use I2C Scanner to check the address, if necessary change the 0x3C in the line below
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  // init done
  display.clearDisplay();
  // set font size
  display.setTextSize(1);  // small font size
  display.setTextColor(WHITE);
  //
  display.setCursor(40,20);
  display.print("OLED Clock");
  display.setCursor(60,30);
  display.print("by");  
  display.setCursor(35,40); 
  display.print("Chris Rouse");
  //  
  //display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  display.display();
  delay(5000);
  display.clearDisplay();
  
}

void loop() {
 //***** RTC **********
  DateTime now = RTC.now();
  display.setCursor(30,57);  
  display.print(now.day(), DEC);
  display.print('/');
  for (int i=0; i<=2; i++){
    monthName[i]=monthString[monthIndex[now.month()-1]+i];
  }
  for (int i=0; i<=2; i++){
   display.print(monthName[i]);
  } 
  display.print('/');
  display.print(now.year(), DEC);   
 // ********************* 

  //
  // display time in digital format
  display.setCursor(10,2); 
  display.print(now.hour(), DEC);
  printDigits(now.minute());
  printDigits(now.second());
  //
  // Now draw the clock face
  display.drawCircle(display.width()/2, display.height()/2, 20, WHITE); 
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
    display.drawLine(x2,y2,x3,y3,WHITE);
  }
  // display second hand
  float angle = now.second()*6 ;
  angle=(angle/57.29577951) ; //Convert degrees to radians  
  int x3=(64+(sin(angle)*(20)));
  int y3=(32-(cos(angle)*(20)));
  display.drawLine(64,32,x3,y3,WHITE);
  //
  // display minute hand
  angle = now.minute() * 6 ;
  angle=(angle/57.29577951) ; //Convert degrees to radians  
  x3=(64+(sin(angle)*(20-3)));
  y3=(32-(cos(angle)*(20-3)));
  display.drawLine(64,32,x3,y3,WHITE);
  //
  // display hour hand
  angle = now.hour() * 30 + int((now.minute() / 12) * 6 )   ;
  angle=(angle/57.29577951) ; //Convert degrees to radians  
  x3=(64+(sin(angle)*(20-11)));
  y3=(32-(cos(angle)*(20-11)));
  display.drawLine(64,32,x3,y3,WHITE);
 //
 // now add temperature if needed
 if (useTMP36==true) {
 //getting the voltage reading from the temperature sensor
   tempReading = analogRead(tempPin);   
 // converting that reading to voltage, for 3.3v arduino use 3.3
   float voltage = tempReading * aref_voltage;
   voltage /= 1024.0; 
 // now print out the temperature
   int temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset                                              //to degrees ((voltage - 500mV) times 100)
   display.setCursor(100,2);
   display.print(temperatureC); 
   display.print("C");
 }
  
  // update display with all data
  display.display();
  delay(100);
  display.clearDisplay();

}

// **************** End Main Loop *****************

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  display.print(":");
  if(digits < 10)
    display.print('0');
    display.print(digits);
  }
