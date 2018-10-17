
/**
* SMARDENS- THE RESILIENT SMART GARDEN
* SMARDUINO 2.0.0
*
* Arduino: DUE
* Summary: Watering Schedule and Hardware Valve/Sensor Test
* The Arduino will obtain data from the Wifi Module ESP 8266 Huzzah through serial communication.
* It will read and store the 5 soil moisture sensor, 2 UV sensor, and 2 Humidity/Temperature sensor readings every three
* hours; therefore it will read and store 8 data readings per sensor per day [3am, 6am, 9am, 12pm, 3pm, 6pm, 9pm, 12am].
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
*  -DHT1PIN: to the digital pin on the Arduino
*  -SOILMOISTUREPIN1: to the analog pin on the Arduino
*  -SOILMOISTUREPIN2: to the analog pin on the Arduino
*  -UV1PIN: to the analog pin on the Arduino
*
* Date: 10/16/18
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
* Software Serial: https://www.arduino.cc/en/Reference/SoftwareSerial
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
const int UV1PIN = 2; //analog
#define RX 0 //digital for serial communication
#define TX 1 //digital for serial communication 
#define DHT1PIN 13 //digital
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
unsigned long duration1 = 1000UL * 60 * REG_WATERING_DURATION;
unsigned long duration2 = 1000UL * 60 * SHORT_WATERING_DURATION;

//Sensor Readings
String sm1;
String sm2;
String sm3;
String sm4;
String sm5;
String uv1;
String uv2;
String ht1;
String ht2;

//Soil Moisture Values
int soil1;
int soil2;
int soil3;
int soil4;
int soil5;

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

//Data and Files
String data;
File dataFile;

//File Name Directory
String homeDir = "Smardens_Garden_Data";

//boolean to establish valid file pathways
bool smardensGarden = false;

void setup()
{
  Serial.begin(9600);

  /*Pin setup*/
  pinMode(SOLENOIDPIN, OUTPUT); //set solenoid pin to control valve
  pinMode(RX, INPUT); //set receiving pin as the input
  pinMode(TX, OUTPUT); //set the transmiting pin as the output
  dht.setup(DHT1PIN); //set dht pin

  /*SD and Directory setup*/
  SDSetup();

  setTime(CURRENT_HOUR, CURRENT_MINUTE, 00, CURRENT_MONTH, CURRENT_DAY, CURRENT_YEAR); // example: 16:00:00 10-10-2018 
}

/*Watering Schedule*/
void impl()
{
  waterSet();
  if (hour() % 3 == 0) {
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

/*Creates the file where the data is written*/
void createFile()
{
  buildDateFolder();

  Serial.println("Arduino is listening");

  data = Serial.readString();
  String idType = data.substring(11, 12); //"ID"
  String idNum = data.substring(13, 14); //"##"

  sensorReading(idType, idNum);

}

/*Senosor readings to be stored in a file and store the previous readings */
void sensorReading(String idType, String idNum)
{
  if (idType.equals("sm")) {
    File dataFile = SD.open(date + "/" + timeCapture + "/" + "SM.txt", FILE_WRITE);
    if (idNum.equals("01")) {
      sm1 = Serial.println(data);
      soil1 = data.substring(30).toInt();
    }
    if (idNum.equals("02")) {
      sm2 = Serial.println(data);
      soil2 = data.substring(30).toInt();
    }
    if (idNum.equals("03")) {
      sm3 = Serial.println(data);
      soil3 = data.substring(30).toInt();
    }
    if (idNum.equals("04")) {
      sm4 = Serial.println(data);
      soil4 = data.substring(30).toInt();
    }
    if (idNum.equals("05")) {
      sm5 = Serial.println(data);
      soil5 = data.substring(30).toInt();
    }
    else {
      Serial.println("id not found");
    }
  }
  if (idType.equals("uv")) {
    File dataFile = SD.open(date + "/" + timeCapture + "/" + "UV.txt", FILE_WRITE);
    if (idNum.equals("01")) {
      uv1 = Serial.println(data);
    }
    if (idNum.equals("02")) {
      uv2 = Serial.println(data);
    }
    else {
      Serial.println("id not found");
    }
  }
  if (idType.equals("ht")) {
    File dataFile = SD.open(date + "/" + timeCapture + "/" + "HT.txt", FILE_WRITE);
    if (idNum.equals("01")) {
      ht1 = Serial.println(data);
    }
    if (idNum.equals("02")) {
      ht2 = Serial.println(data);
    }
    else {
      Serial.println("id not found");
    }
  }
  else {
    Serial.println("error opening txt");
  }

  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
  }
}

/*Moisture level check as dry, moist, wet*/
String checkDry(int dryness)
{
  const int AirValue = 785;
  const int WaterValue = 391;
  int intervals = (AirValue - WaterValue) / 3;
  if (dryness > WaterValue && dryness < (WaterValue + intervals))
  {
    return "Wet";
  }
  else if (dryness >(WaterValue + intervals) && dryness < (AirValue - intervals))
  {
    return "Moist";
  }
  else if (dryness < AirValue && dryness >(AirValue - intervals))
  {
    return "Dry";
  }
}

/*Normal watering schedule with normal water amount*/
void firstWater(int duration)
{
  if (hour() == WATERING_TIME_1 && minute() == 0 && second() == 0)
  {
    digitalWrite(SOLENOIDPIN, HIGH); //open valve
    delay(duration);
    digitalWrite(SOLENOIDPIN, LOW); //close valve
  }
}

/*Second watering schedule if needed*/
void secondWater(int duration)
{
  if (hour() == WATERING_TIME_2 && minute() == 0 && second() == 0)
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


  dryness = (soil1 + soil2 + soil3 + soil4 + soil5) / 5; //average of 5 soil moisture level

  //5 minute before normal watering schedule, check the soil moisture level to determine length of watering
  if (hour() == WATERING_TIME_2 - 1 && minute() == 55 && second() == 0) {
    check = checkDry(dryness);
    if (check == "Dry") {
      currentDuration = REG_WATERING_DURATION;
    }
    else if (check == "Moist") {
      currentDuration = SHORT_WATERING_DURATION;
    }
    else {
      currentDuration = 0; //don't open if the field is super wet
    }
  }
  
  // second watering check time 
  if (hour() == WATERING_TIME_2 - 1 && minute() == 55 && second() == 0) {
    //check last soil moisture level
    dryness = (soil1 + soil2 + soil3 + soil4 + soil5) / 5; //average of 5 soil moisture level
    check = checkDry(dryness);
    if (check == "Dry") {
      currentDuration = SHORT_WATERING_DURATION;
    }
    else {
      currentDuration = 0; //don't open if the field is super wet
    }
  }
  
  firstWater(currentDuration);
  secondWater(currentDuration);
}

void test() {
  digitalWrite(SOLENOIDPIN, HIGH); //open valve
  delay(1000 * 30); //1000 = 1 second
  digitalWrite(SOLENOIDPIN, LOW); //close valve
}

void loop()
{
  //test();
  impl();
}
