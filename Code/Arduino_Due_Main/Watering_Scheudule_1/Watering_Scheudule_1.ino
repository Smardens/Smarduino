/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("Hello, world?");
}

void loop() { // run over and over
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}

/**
 * SMARDENS- THE RESILIENT SMART GARDEN 
 * SMARDUINO 1.0.0
 * 
 * Arduino: DUE
 * Summary: Watering Schedule and Hardware Valve/Sensor Test
 * The Arduino will read and store the 2 soil moisture sensors, 1 UV sensor, and 1 Humidity/Temperature sensor data
 * every three hours; therefore it will read and store 8 data readings per day [3am, 6am, 9am, 12pm, 3pm, 6pm, 9pm]. 
 * Based on the algorithm to determine whether to water, the Arduino is programmed to water at time1 for a regular 
 * water, and if the soil is still dry it will water again at time2.
 * 
 * Notes:
 * To customize the schedule, change the following variables:
 *  -DATE: to the current date when the Arduino is powered
 *  -TIME: to the current time when the Arduino is powered
 *  -REG_WATERING_DURATION: to the number of minutes the garden is watered if the soil is dry
 *  -SHORT_WATERING_DURATION: to the number of minutes the garden is watered if the soil is moist
 *  
 * To customize the pin inputs, change the following variables:
 *  -SOLENOIDPIN: to the digital pin on the Arduino
 *  -DHTPIN: to the digital pin on the Arduino
 *  -SOILMOISTUREPIN1: to the analog pin on the Arduino
 *  -SOILMOISTUREPIN2: to the analog pin on the Arduino
 *  -UVPIN: to the analog pin on the Arduino
 *  
 * Date: 10/11/18
 * 
 * Created by: 
 * Caitlin Rubia
 * Xinyi Chen
 * Brian Powell
 * Israel Grogin
 * 
 * Reference:
 * https://smardens.com/
 * 
 * Resources:  
 * Clock: https://playground.arduino.cc/code/time
 * SD Card Module: https://www.arduino.cc/en/Reference/SD
 * Humidity/ Temperature Sensor: https://github.com/markruys/arduino-DHT
 * 
 * Modified Date:
 * 
 * Modified by:
 * 
 */
#include <TimeLib.h>
#include <SD.h>
#include <DHT.h>

//Pin Locations
const int SOILMOISTURE1PIN = 0; //analog
const int SOILMOISTURE2PIN = 1; //analog
const int UVPIN = 2; //analog
#define DHTPIN 13 //digital
#define SOLENOIDPIN 11 //digital
#define SDPIN 9 //digital

//Custom Values
int REG_WATERING_DURATION = 10; //10- minuntes adjusted for slow drip for a regular watering duration
int SHORT_WATERING_DURATION = 6; //6- minutes for a shorter watering duration

int WATERING_TIME_1 = 8; //8AM - hour of the day in the morning (regular watering time)
int WATERING_TIME_2 = 18; //6PM- hour of the day in the afternoon (extra watering time)

int CURRENT_MONTH = 10;//current month
int CURRENT_DAY = 16;//current day
int CURRENT_YEAR = 2018;//current year

int CURRENT_HOUR = 7; //current hour of the day
int CURRENT_MINUTE = 40;//current minute of the day

//Using class to capture both humidity and temperature values
DHT dht;

//Durations
unsigned long duration1=1000UL*60*REG_WATERING_DURATION; 
unsigned long duration2=1000UL*60*SHORT_WATERING_DURATION; 

//Sensor Readings
int sm1 = 0;
int sm2 = 0;
int uv = 0;
float temp = 0;
float hum = 0;

//Date
String date;
int dayC;
int monthC;
int yearC;

//Time
String timeCapture;
int hourC;
int minuteC;
int secondC;

//File Name Directory
String homeDir = "Smardens_Garden_Data";

//boolean to establish valid file pathways
bool smardensGarden = false; 

void setup()
{
  Serial.begin(9600);

  /*Pin setup*/
  pinMode(SOLENOIDPIN, OUTPUT); //set solenoid pin to control valve
  dht.setup(DHTPIN); //set dht pin
  
  /*SD and Directory setup*/
  SDSetup();
  
  setTime(CURRENT_HOUR, CURRENT_MINUTE,00,CURRENT_MONTH,CURRENT_DAY,CURRENT_YEAR); // example: 16:00:00 10-10-2018 
}

/*Watering Schedule*/
void impl()
{
    waterSet();
    if (hour()%3==0){
        createFile();
    }
}

/*Initializing SD card*/
void SDSetup()
{
      while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(SDPIN)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  buildDirectory();
}

/*Checks if pathway exists*/
bool buildDir(String pathway)
{
    if (!SD.exists(pathway)) {
      smardensGarden = SD.mkdir(pathway);
    }
    return smardensGarden;
}

/*Getting the current date*/
String getSetDate() 
{
    // get date to check folder directory
    dayC = day();
    monthC = month();
    yearC = year();
    // convert date variables to string date
    date = String(dayC) + "_" + String(monthC) + "_" + String(yearC);
    return date;
}

/*Create Home Folder*/
void buildDirectory()
{
    // intializing home directory
    if (buildDir(homeDir)) {
      Serial.println("Creating new Smarden Folder");
    }
    else {
      Serial.println("Error creating new Smarden Folder");
    }
}

/*Getting the current time*/
String getSetTime() 
{
    // get time to check folder directory
    hourC = hour();
    minuteC = minute();
    secondC = second();
    // convert date variables to string date
    timeCapture = String(hourC) + "_" + String(minuteC) + "_" + String(secondC);
    return timeCapture;
}

/*Creates the date folder containing the files*/
void buildDateFolder()
{
   date = getSetDate();
    // create sub-directory based on date if it doesn't exist
    if (buildDir(homeDir + "/" + date)) {
      Serial.println("Creating new Date Folder");
    }
    else { //add to the existing sub-directory
      Serial.println("Date Folder alread exists");
    }

}

/*Senosor readings to be stored in a file */
String sensorReading(int pinName)
{
    String dataValue = "0";
    if(pinName == SOILMOISTURE1PIN){//soil moisture 1
       sm1 =  analogRead(SOILMOISTURE1PIN);
       dataValue =  "Soil Moisture 1: " + String();
    }
    if(pinName == SOILMOISTURE2PIN){//soil moisture 2
       dataValue =  "Soil Moisture 2: " + String(analogRead(SOILMOISTURE2PIN));
    }
    if(pinName == UVPIN){//UV
        dataValue =  "UV: "+ String(analogRead(UVPIN));
    }
    if(pinName == DHTPIN){
        String humidity;
        String temperature;
        temp = dht.getTemperature();//DHT temperature
        hum = dht.getHumidity();//DHT humidity
        humidity =  String(hum);
        temperature = String(temp);
        dataValue = "DHT: Humidity- " + humidity + "Temperature: " + temperature;
    }
    return dataValue;
}

/*Creates the time file that will be written to*/
void createFile()
{
    String sm1;
    String sm2;
    String uv1;
    String ht1;

    buildDateFolder();
    timeCapture = getSetTime();
    
    sm1 = Serial.println(sensorReading(SOILMOISTURE1PIN));
    sm2 = Serial.println(sensorReading(SOILMOISTURE1PIN));
    uv1 = Serial.println(sensorReading(UVPIN));
    ht1 = Serial.println(sensorReading(DHTPIN));

    File dataFile = SD.open( date + "/" + timeCapture + ".txt", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) {
        dataFile.println(sm1);
        dataFile.println(sm2);
        dataFile.println(uv1);
        dataFile.println(ht1);
        dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.println("error opening datalog.txt");
    }
}

/*Moisture level check as dry, moist, wet*/
String checkDry(int dryness)
{
  const int AirValue = 785;
  const int WaterValue = 391;
  int intervals = (AirValue - WaterValue)/3;
  if(dryness > WaterValue && dryness < (WaterValue + intervals))
  {
    return "Wet";
  }
  else if(dryness > (WaterValue + intervals) && dryness < (AirValue - intervals))
  {
    return "Moist";
  }
  else if(dryness < AirValue && dryness > (AirValue - intervals))
  {
    return "Dry";
  }
}

/*Normal watering schedule with normal water amount*/
void firstWater(int duration)
{
    if(hour()== WATERING_TIME_1 && minute()==0 && second()==0)
  {
     digitalWrite(SOLENOIDPIN, HIGH); //open valve
     delay(duration) ;
     digitalWrite(SOLENOIDPIN, LOW); //close valve
  }
}

/*Second watering schedule if needed*/
void secondWater(int duration)
{
  if(hour()== WATERING_TIME_2 && minute()==0 && second()==0)
  {
     digitalWrite(SOLENOIDPIN, HIGH); //open valve
     delay(duration); //1000 = 1 second
     digitalWrite(SOLENOIDPIN, LOW); //close valve
  }
}

/*Checking the soil moisture level 5 minutes before scheduled watering time to determine 
water durations and number of times watered*/
void waterSet()
{
    int currentDuration;
    int dryness; 
    String check;
    
    sm1 = analogRead(SOILMOISTURE1PIN); //soil moisture 1 reading 
    sm2 = analogRead(SOILMOISTURE2PIN); //soil moisture 2 reading 
    dryness = (sm1+sm2)/2; //average of 2 soil moisture level

    //5 minute before normal watering schedule, check the soil moisture level to determine length of watering
    if(hour()==WATERING_TIME_2-1 && minute()==55 && second()==0){
        check = checkDry(dryness);
        if(check == "Dry"){
            currentDuration = REG_WATERING_DURATION;
        }
        else if(check == "Moist"){
            currentDuration = SHORT_WATERING_DURATION;
        }
        else{
            currentDuration = 0; //don't open if the field is super wet
        }
    }
    // second watering check time 
    if(hour()== WATERING_TIME_2-1 && minute()==55 && second()==0){
      //check last soil moisture level
      dryness = (sm1+sm2)/2; //average of 5 soil moisture level
      check = checkDry(dryness);
      if(check == "Dry"){
        currentDuration = SHORT_WATERING_DURATION; 
      }
      else{
        currentDuration = 0; //don't open if the field is super wet
      }
    }
    firstWater(currentDuration);
    secondWater(currentDuration); 
}

void test(){
   digitalWrite(SOLENOIDPIN, HIGH); //open valve
   delay(1000*30); //1000 = 1 second
   digitalWrite(SOLENOIDPIN, LOW); //close valve
}

void loop() 
{
 //test();
 impl();
}
