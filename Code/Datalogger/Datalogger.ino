/*
  SD card datalogger

 This example shows how to log data from three analog sensors
 to an SD card using the SD library.

 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created  24 Nov 2010
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>
#include <TimeLab.h>

const int chipSelect = 4;
String date;
String timeCapture;

int dayC;
int monthC;
 int yearC;
int hourC;
int minuteC;
int secondC;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

void getSetDate() {
    // get date to check folder directory
    dayC = day();
    monthC = month();
    yearC = year();
    // convert date variables to string date
    date = String(dayC) + "_" + String(monthC) + "_" + String(yearC);
    return date;
  }
void getSetTime() {
    // get time to check folder directory
    hourC = hour();
    minuteC = minute();
    secondC = second();
    // convert date variables to string date
    timeCapture = String(hourC) + "_" + String(minuteC) + "_" + String(secondC);
    return timeCapture;
  }

void loop() {
  // make a string for assembling the data to log:
  String dataString;
  String sm1;
  String sm2;
  String uv;
  String h;

  // read three sensors
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    int sensor = analogRead(analogPin);
    if (analogPin == 0){
      sm1 = "Soil Moistrure: " + String(sensor);
    }
    else if (analogPin == 1){
      sm2 = "Soil Moistrure: " + String(sensor);
    }
    else{
      uv = "UV: " + String(sensor);
    }
    
  }

  int digitaPin = 5;
  int sensor =  digitalRead(digitalPin);
  h = "Humidity: " + String(sensor);

  dataString = getSetDate() + " - " + getSetTime() + " : " + sm1 + "," + sm2 + "," + uv + "," + h;

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}









