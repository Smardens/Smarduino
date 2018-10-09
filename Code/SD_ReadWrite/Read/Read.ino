
/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>

//global variables
bool smardensGarden = false;
File data;
String homeDir = "Smardens_Garden_Data";
String date;
String timeCapture;
String sensorID;
String dataReading;

int dayC;
int monthC;
int yearC;
int hourC;
int minuteC;
int secondC;

const int chipSelectPin = 52; //initialize the bus for a device on pin 52

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelectPin)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  buildDirectory();
  data = SD.open(homeDir + "/" + date + "/" + timeCapture + "/Summary.txt", FILE_WRITE);
  createSummary(data);

  createFile(id, sm);
  createFile(id, hum);
  createFile(id, temp);
  createFile(id, uv);
}

bool buildDir(String pathway) {
    if (!SD.exists(pathway)) {
      smardensGarden = SD.mkdir(pathway);
    }
    return smardensGarden;
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

void buildDirectory() {
    // intializing home directory
    if (buildDir(homeDir)) {
      Serial.println("Creating new Smarden Folder");
    }
    else {
      Serial.println("Error creating new Smarden Folder");
    }

    getSetDate();

    // create sub-directory based on date if it doesn't exist
    if (buildDir(homeDir + "/" + date)) {
      Serial.println("Creating new Date Folder");
    }
    else { //add to the existing sub-directory
      Serial.println("Date Folder alread exists");
    }

    getSetTime();

    // create sub-directory based on time
    // time should be Military time
    if (buildDir(homeDir + "/" + date + "/" + timeCapture)) {
      Serial.println("Creating new Smarden Folder");
    }
    else {
      Serial.println("Error creating new Smarden Folder");
    }
  }

// write a summary page
void createSummary(File data) {
    if (data) {
      Serial.println("Writing to " + data);
      data.println("Date: " + date);
      data.println("Time: " + time);
      data.println("SENSOR ID || DATA READING");
      data.close();
    }
    else {
      Serial.println("Error opening file: " + data);
    }
  }

// creating/opening files for each sensor
void createFile(String id, String reading) {
    sensorID = id;
    data = SD.open(homeDir + "/" + date + "/" + timeCapture + "/" + sensorID + ".txt", FILE_WRITE);
    writeToFile(data);
    data = SD.open(homeDir + "/" + date + "/" + timeCapture + "/Summary.txt", FILE_WRITE);
    addToSummary(data);
  }
  
// writing to the file
void writeToFile(File data) {
    if (data) {
      dataReading = reading;
      data.println(dataReading);
    }
    else {
      Serial.println("Arduino can't find message to be stored");
    }
    data.close();
  }
  
//adding to the summary page
void addToSummary(File data) {
    if (data) {
      Serial.println("Writing to Summary File");
      data.println("  " + sensorID + "      ||     " + dataReading);
      data.close();
    }
    else {
      Serial.println("Error opening Summary File");
    }
  }
void loop() {
  // put your main code here, to run repeatedly:

}


