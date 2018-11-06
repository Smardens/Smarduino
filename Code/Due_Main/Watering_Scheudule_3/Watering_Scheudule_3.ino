/**
SMARDENS- THE RESILIENT SMART GARDEN
SMARDUINO 2.1.0

Arduino: DUE
Summary: Watering Schedule and Hardware Valve/Sensor Test
The Arduino will obtain data from the Wifi Module ESP 8266 Huzzah through serial communication.
It will read and store the 5 soil moisture sensor, 2 UV sensor, and 2 Humidity/Temperature sensor
readings every three hours; therefore it will read and store 8 data readings per sensor per day.
Based on the algorithm to determine whether to water, the Arduino is programmed to water at time1
for a regular water, and if the soil is still dry it will water again at time2.

Notes:
To customize the schedule, change the following variables:
-REG_WATERING_DURATION: to the number of minutes the garden is watered if the soil is dry
-SHORT_WATERING_DURATION: to the number of minutes the garden is watered if the soil is moist
-WATERING_TIME_1: to the hour of the day for the regular watering time (ideally morning, ie: 8AM, enter 8)
-WATERING_TIME_2: to the hour of the day for the extra watering time (ideally afternoon; ie: 5PM, enter 17)
-CURRENT_MONTH: setting the month
-CURRENT_DAY: setting the day
-CURRENT_YEAR: setting the year
-YEAR_ABBR: second part of the current year (ie: year 2018, enter 18)
-CURRENT_HOUR: setting the hour
-CURRENT_MINUTE: setting the minutes


To customize the pin inputs, change the following variables:
-SOLENOIDPIN: to the digital pin on the Arduino
-SDPIN: to digital pin on arduino

Date: 10/26/18

Created by:
Caitlin Rubia
Xinyi Chen
Brian Powell
Israel Grogin

Reference:
https://smardens.com/

Resources:
Clock: https://playground.arduino.cc/code/time
SD Card Module: https://www.arduino.cc/en/Reference/SD
SPI: https://www.arduino.cc/en/Reference/SPI

Modified Date:

Modified by:

*/
#include <TimeLib.h>
#include <SD.h>
#include <SPI.h>

//Define Pin Locations
#define SOLENOIDPIN 11 //digital
#define SDPIN 9 //digital

//Custom Values
int REG_WATERING_DURATION = 10; //10- minuntes for a regular watering duration (adjusted for weak drip irrigation)
int SHORT_WATERING_DURATION = 6; //6- minutes for a shorter watering duration (adjusted for weak drip irrigation)
int WATERING_NOTKNOWN = 5; // 5- minutes for watering when sensor readings don't fit within the intervals 


int WATERING_TIME_1 = 8; //8AM - hour of the day in the morning (regular watering time)
int WATERING_TIME_2 = 18; //6PM- hour of the day in the afternoon (extra watering time)

int CURRENT_MONTH = 10;//current month
int CURRENT_DAY = 30;//current day
int CURRENT_YEAR = 2018;//current year
int YEAR_ABBR = 18; //second part of the current year (ie: year 2018, enter 18)
int CURRENT_HOUR = 4; //current hour of the day
int CURRENT_MINUTE = 40;//current minute of the day

//Soil Moisture value determinants for dryness
int soil1, soil2, soil3, soil4, soil5;

//Durations for Watering
unsigned long duration1 = 1000UL * 60 * REG_WATERING_DURATION;
unsigned long duration2 = 1000UL * 60 * SHORT_WATERING_DURATION;

//Determinant Values for Soil Moisture Dryness Intervals
const int AIRVALUE = 785;
const int WATERVALUE = 391;

//Foler and File Creation for Sensor Readings
String data; //the sensor date taht will be stored
File dataFile; //txt. file being created
const char *rootPath = "Smardens"; //root folder name- Smardens
char dirName[100]; //filepath size allocated 
String path = ""; //filepath name
bool smardensGarden = false; //boolean to establish valid file pathways

void setup()
{
  /*Setting the current time*/
  setTime(CURRENT_HOUR, CURRENT_MINUTE, 00, CURRENT_DAY, CURRENT_MONTH, CURRENT_YEAR); // example: 16:00:00 18-10-2018

  /*Serial setup*/
  Serial.begin(115200); //Arduino Due Serial
  Serial1.begin(115200); //WIFI Module ESP 8266 Huzzah Serial for TX1, RX1

  /*Pin setup*/
  pinMode(SOLENOIDPIN, OUTPUT); //set solenoid pin to control valve

  /*SD and Directory setup*/
  SDSetup();
  buildRoot();
 
  /*Validate water solenoid valve is working with irrigation*/
  //initialWaterTest();
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
  Serial.println("Card Initialized.");
}

/*Checks if filepath is valid to be created*/
bool buildValid(const char *dir)
{
  smardensGarden = false;
  if (!SD.exists(dir)) {
    smardensGarden = SD.mkdir(dir);
  }
  return smardensGarden;
}

/*Create the root folder*/
void buildRoot()
{
  if (buildValid(rootPath)) {
    Serial.println("Creating Smardens Folder");
  }
  else {
    Serial.println("Error- Can't Creating Smardens Folder");
  }
}

/*Creates the date/time folder containing the files*/
void buildSubRoot()
{
  sprintf(dirName, "%s/%d_%d_%d/%d_%d", rootPath, month(), day(), YEAR_ABBR, hour(), minute());
  if (buildValid(dirName)) {
    Serial.println("Creating Date/Time Folder");
    path = String(dirName); //the filepath that contains the mm_dd_yy folder and the hh_mm_ss folder in it
  }
  else {
    Serial.println("Error- Can't Creating Date/Time Folder");
  }
}

/*Senosor readings to be stored in a file */
void sensorReading(String idType, String idNum)
{
  if (idType.equals("SM")) {
    dataFile = SD.open(path + "/SM.txt", FILE_WRITE);
    if (idNum.equals("01")) {
      Serial.println("SM01 written");
      soil1 = data.substring(30).toInt();
    }
    else if (idNum.equals("02")) {
      Serial.println("SM02 written");
      soil2 = data.substring(30).toInt();
    }
    else if (idNum.equals("03")) {
      Serial.println("SM03 written");
      soil3 = data.substring(30).toInt();
    }
    else if (idNum.equals("04")) {
      Serial.println("SM04 written");
      soil4 = data.substring(30).toInt();
    }
    else if (idNum.equals("05")) {
      Serial.println("SM05 written");
      soil5 = data.substring(30).toInt();
    }
    else {
      Serial.println("idType = SM; idNum not found");
    }
  }
  else if (idType.equals("UV")) {
    dataFile = SD.open(path + "/UV.txt", FILE_WRITE);
    if (idNum.equals("01")) {
      Serial.println("UV01 written");
    }
    else if (idNum.equals("02")) {
      Serial.println("UV02 written");
    }
    else {
      Serial.println("idType = UV; idNum not found");
    }
  }
  else if (idType.equals("HT")) {
    dataFile = SD.open(path + "/HT.txt", FILE_WRITE);
    if (idNum.equals("01")) {
      Serial.println("HT01 written");
    }
    else if (idNum.equals("02")) {
      Serial.println("HT02 written");
    }
    else {
      Serial.println("idType = HT; idNum not found");
    }
  }
  else {
    Serial.println("idType not found");
  }

  if (dataFile) {
    Serial.println("Text file is open and will be written in");
    dataFile.println(data);
    dataFile.close();
  }
  else {
    Serial.println("Text file couldn't be opened");
  }
}

/*Moisture level check as dry, moist, wet*/
String checkDry(int dryness)
{
  int intervals = (AIRVALUE - WATERVALUE) / 3;
  if (dryness > WATERVALUE && dryness < (WATERVALUE + intervals))
  {
    return "Wet";
  }
  else if (dryness >(WATERVALUE + intervals) && dryness < (AIRVALUE - intervals))
  {
    return "Moist";
  }
  else if (dryness < AIRVALUE && dryness >(AIRVALUE - intervals))
  {
    return "Dry";
  }
  else return "Unknown"; // handles when dryness doesnt fit within the intervals
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
    else if (check == "Wet"){
      currentDuration = 0; //don't open if the field is super wet
    }
    else if (check == "Unknown"){
      currentDuration = WATERING_NOTKNOWN; 
    }
    else{
      currentDuration = WATERING_NOTKNOWN; //handles for precaution
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

/*Test Water Valve*/
void initialWaterTest() {
  digitalWrite(SOLENOIDPIN, HIGH); //open valve
  delay(1000 * 60 * WATERING_NOTKNOWN); //1000 = 1 second
  digitalWrite(SOLENOIDPIN, LOW); //close valve
}

/*Sensor Readings from Wifi Module ESP 8266 Huzzah- Access Point*/
void waterSystem() {
  waterSet(); //waters at certain time for certain durations
  while (Serial1.available() > 0) { //reading from WIFI Module ESP 8266 Huzzah acting as Access Point
    buildSubRoot(); //builds subfolders based on the current date and time
    Serial.println("Arduino is listening");
    data = Serial1.readString(); //String data = "Sensor ID: TY##" 
    String idType = data.substring(11, 13); //"TY"
    String idNum = data.substring(13, 15); //"##"
    Serial.println("idType: " + idType);
    Serial.println("idNum: " + idNum);
    sensorReading(idType, idNum);
  }
}

void loop()
{
  waterSystem();
}
