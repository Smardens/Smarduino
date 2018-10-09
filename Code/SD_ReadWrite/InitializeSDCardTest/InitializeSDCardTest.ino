/*
  SD card basic file example

 This example shows how to create and destroy an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 1
 ** MISO - pin 4
 ** SCK - pin 3
 ** CS - pin 52 (for MKRZero SD: SDCARD_SS_PIN)
 ** 5V - pin 2 (+Vcc) 
 ** GND - pin 6 

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 modified 9 Oct 2018 for Arduino DUE use
 by Caitlin Rubia
 referrence: https://forum.arduino.cc/index.php?topic=236903.0

 Note: Only use SD Cards that are of size: 1GB or 2GB
 Format the SD Card with : https://www.sdcard.org/downloads/formatter_4/
 Check SD Card Info before initializaing files: Go to Arduino IDE > File > Examples > SD > CardInfo
 */
#include <SPI.h>
#include <SD.h>

File myFile;
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

  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }

  // open a new file and immediately close it:
  Serial.println("Creating example.txt...");
  myFile = SD.open("example.txt", FILE_WRITE);
  myFile.close();

  // Check to see if the file exists:
  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }

  // delete the file:
  Serial.println("Removing example.txt...");
  SD.remove("example.txt");

  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
  } else {
    Serial.println("example.txt doesn't exist.");
  }
}

void loop() {
  // nothing happens after setup finishes.
}



