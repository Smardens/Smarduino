/*
  Adapted by Brian Powell
  CECS 491B CSULB
  Birgit Penzenstadler
*/

#include <ESP8266WiFi.h>
#include "EIoTCloudRestApiV1.0.h"
#include <EEPROM.h>
#include <DHT.h>

#define DEBUG

#ifdef DEBUG
#define DPRINTLN(x)  Serial.println(x)
#define DPRINT(x)    Serial.print(x)
#else
#define DPRINTLN(x)
#define DPRINT(x)
#endif

EIoTCloudRestApi eiotcloud;

// change those lines
#define WiFi_USERNAME "" //need workaround for beachnet+
#define WiFi_PASSWORD "" //also need workaround for beachnet+
#define INSTANCE_ID "5b8ef14047976c47a0d13172"
#define TOKEN "6COk0fuparVMijTPzY0hjoFMCwvJGPgGxB33TpIw"

#define CONFIG_START 32
#define CONFIG_VERSION "v01"

#define REPORT_INTERVAL 60 //in seconds

struct StoreStruct {
  // This is for mere detection if they are your settings
  char version[4];
  // The variables of your settings
  char token[41];
  uint moduleId;
  //bool tokenOk; // valid token
} storage = {
  CONFIG_VERSION,
  // token
  "6COk0fuparVMijTPzY0hjoFMCwvJGPgGxB33TpIw",
  // The default module 0 - invalid module
  0,
  //0 // not valid
};

float oldTemp;
float oldHum;

String moduleId = "";
String parameterId1 = "";
String parameterId2 = "";

DHT dht;

void setup() {
  Serial.begin(115200);
  DPRINTLN("Start...");
//
  EEPROM.begin(512);
//  loadConfig();
//
//  eiotcloud.begin(WiFi_USERNAME, WiFi_PASSWORD);
//  //eiotcloud.SetToken(TOKEN);
//
//  Serial.println("Connecting to ");
//  Serial.print(WiFi_USERNAME); Serial.println("...");
//
//  int i = 0;
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.print(++i); Serial.print(' ');
//  }
//
//  Serial.print('\n');
//  Serial.println("Connection Established");
//  Serial.print("IP Address:\t");
//  Serial.println(WiFi.localIP());
//  Serial.println("ModuleId:");
//  Serial.println(storage.moduleId);

  // if first time get new token and register new module
  // here hapend Plug and play logic to add module to Cloud
//  if (storage.moduleId == 0)
//  {
//    // get new token - alternarive is to manually create token and store it in EEPROM
//    String token = eiotcloud.TokenNew(INSTANCE_ID);
//    DPRINT("Token: ");
//    DPRINTLN(token);
//    eiotcloud.SetToken(token);
//
//    // remember token
//    token.toCharArray(storage.token, 41);
//
//    // add new module and configure it
//    moduleId = eiotcloud.ModuleNew();
//    DPRINT("ModuleId: ");
//    DPRINTLN(moduleId);
//    storage.moduleId = moduleId.toInt();
//
//    // set module type
//    bool modtyperet = eiotcloud.SetModulType(moduleId, "MT_GENERIC");
//    DPRINT("SetModulType: ");
//    DPRINTLN(modtyperet);
//
//    // set module name
//    bool modname = eiotcloud.SetModulName(moduleId, "Temp_Hum_01");
//    DPRINT("SetModulName: ");
//    DPRINTLN(modname);
//
//    // add image settings parameter
//    String parameterImgId = eiotcloud.NewModuleParameter(moduleId, "Settings.Icon1");
//    DPRINT("parameterImgId: ");
//    DPRINTLN(parameterImgId);
//
//    // set module image
//    bool valueRet1 = eiotcloud.SetParameterValue(parameterImgId, "humidity.png");
//    DPRINT("SetParameterValue: ");
//    DPRINTLN(valueRet1);
//
//    // now add parameter to display temperature
//    parameterId1 = eiotcloud.NewModuleParameter(moduleId, "Sensor.Parameter1");
//    DPRINT("ParameterId1: ");
//    DPRINTLN(parameterId1);
//
//    //set parameter description
//    bool valueRet2 = eiotcloud.SetParameterDescription(parameterId1, "Temperature");
//    DPRINT("SetParameterDescription: ");
//    DPRINTLN(valueRet2);
//
//    //set unit
//    // see http://meyerweb.com/eric/tools/dencoder/ how to encode °C
//    bool valueRet3 = eiotcloud.SetParameterUnit(parameterId1, "%C2%B0C");
//    DPRINT("SetParameterUnit: ");
//    DPRINTLN(valueRet3);
//
//    //Set parameter LogToDatabase
//    bool valueRet4 = eiotcloud.SetParameterLogToDatabase(parameterId1, true);
//    DPRINT("SetLogToDatabase: ");
//    DPRINTLN(valueRet4);
//
//    //SetAverageInterval
//    bool valueRet5 = eiotcloud.SetParameterAverageInterval(parameterId1, "10");
//    DPRINT("SetAverageInterval: ");
//    DPRINTLN(valueRet5);
//
//    //adding parameter to display humidity
//    //easyIoT won't allow us to send two parameters untill we buy it, this code will be commented out
//    /*
//      parameterId2 = eiotcloud.NewModuleParameter(moduleId, "Sensor.Parameter2");
//      DPRINT("parameterId2: ");
//      DPRINTLN(parameterId2);
//
//      //set parameter description
//      bool valueRet6 = eiotcloud.SetParameterDescription(parameterId2, "Humidity");
//      DPRINT("SetParameterDescription: ");
//      DPRINTLN(valueRet2);
//
//      //set unit
//      bool valueRet7 = eiotcloud.SetParameterUnit(parameterId2, "%");
//      DPRINT("SetParameterUnit: ");
//      DPRINTLN(valueRet7);
//
//      //Set parameter LogToDatabase
//      bool valueRet8 = eiotcloud.SetParameterLogToDatabase(parameterId2, true);
//      DPRINT("SetLogToDatabase: ");
//      DPRINTLN(valueRet8);
//
//      //SetAvreageInterval
//      bool valueRet9 = eiotcloud.SetParameterAverageInterval(parameterId2, "10");
//      DPRINT("SetAvreageInterval: ");
//      DPRINTLN(valueRet9);
//    */
//    // save configuration
//    saveConfig();
//  }
//
//  // if something went wrong, wait here
//  if (storage.moduleId == 0)
//    delay(1);
//
//  // read module ID from storage
//  moduleId = String(storage.moduleId);
//  // read token ID from storage
//  eiotcloud.SetToken(storage.token);
//  // read Sensor.Parameter1 ID from cloud
//  parameterId1 = eiotcloud.GetModuleParameterByName(moduleId, "Sensor.Parameter1");
//  DPRINT("parameterId1: ");
//  DPRINTLN(parameterId1);

  // read Sensor.Parameter2 ID from cloud
  /*
    parameterId2 = eiotcloud.GetModuleParameterByName(moduleId, "Sensor.Parameter2");
    DPRINT("parameterId2: ");
    DPRINTLN(parameterId2);
  */

  Serial.println();
  Serial.println("Status\tTemperature(C)\t(F)\tHumidity (%)");

  dht.setup(2); //data pin 2

  oldTemp = -1;
  oldHum = -1;
}


void loop() {
  float hum = dht.getHumidity();
  float temp = dht.getTemperature();

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(temp);
  Serial.print("\t\t");
  Serial.print(hum);
  Serial.print("\t\t");
  //Serial.println(dht.toFahrenheit(temp), 1);

//  if (temp != oldTemp) //(temp != oldTemp || hum != oldHum)
//  {
//    //send temperature
//    eiotcloud.SetParameterValues("[{\"Id\": \"" + parameterId1 + "\", \"Value\": \"" + String(temp) + "\" }]");
//    //eiotcloud.SetParameterValues("[{\"Id\": \""+parameterId1+"\", \"Value\": \""+String(temp)+"\" },{\"Id\": \""+parameterId2+"\", \"Value\": \""+String(hum)+"\" }]");
//
//    oldTemp = temp;
//    //oldHum = hum;
//  }

  int cnt = REPORT_INTERVAL;

  while (cnt--)
    delay(1000);
}


void loadConfig() {
  // To make sure there are settings, and they are YOURS!
  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
    for (unsigned int t = 0; t < sizeof(storage); t++)
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);
}


void saveConfig() {
  for (unsigned int t = 0; t < sizeof(storage); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));

  EEPROM.commit();
}
