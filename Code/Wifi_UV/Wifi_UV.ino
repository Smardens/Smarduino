#include <ESP8266WiFi.h>
#include "EIoTCloudRestApiV1.0.h"
#include <EEPROM.h>

#define DEBUG_PROG

#ifdef DEBUG_PROG
#define DEBUG_PRINTLN(x)  Serial.println(x)
#define DEBUG_PRINT(x)    Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

EIoTCloudRestApi eiotcloud;

//change lines for wifi name and password
#define WiFi_USERNAME "" //change
#define WiFi_PASSWORD "" //change
#define INSTANCE_ID "5b8ef14047976c47a0d13172" 

#define REPORT_INTERVAL 60 //in seconds

#define CONFIG_START 0
#define CONFIG_VERSION "v01"

#define MIN_VAL 20
#define MAX_VAL 240

#define SENSOR_PIN A//uv sensor is on the analog input

struct StoreStruct {
  //this is for mere detection if they are your settings
  char version[4];
  //variables of your settings
  char token[41];
  uint moduleId;
  //bool TokenOk; //valid token
} storage = {
  CONFIG_VERSION,
  //token 
  "1234567890123456789012345678901234567890",
  //the default module 0 - invalid module
  0,
};

String moduleId = "";
String parameterId1 = "";

float lightReading;

void setup() {
  Serial.begin(115200);
  DEBUG_PRINTLN("Start...");

  EEPROM.begin(512);
  loadConfig();

  eiotcloud.begin(WiFi_USERNAME, WiFi_PASSWORD);

  //if first time get new token and register new module 
  //here happened Plug and Play logic to add module to cloud
  if (storage.moduleId == 0)
  {
    //get new token - alternative is to manually create token and store it in eeprom
    String token = eiotcloud.TokenNew(INSTANCE_ID);
    DEBUG_PRINT("Token: ");
    DEBUG_PRINTLN(token);
    eiotcloud.SetToken(token);

    //remember token
    token.toCharArray(storage.token, 41);

    //add new module and configure it
    moduleId = eiotcloud.ModuleNew();
    DEBUG_PRINT("ModuleId: ");
    DEBUG_PRINTLN(moduleId);
    storage.moduleId = moduleId.toInt();
   
    // set module type
    bool modtyperet = eiotcloud.SetModulType(moduleId, "MT_GENERIC");
    DEBUG_PRINT("SetModulType: ");
    DEBUG_PRINTLN(modtyperet);

    // set module name
    bool modname = eiotcloud.SetModulName(moduleId, "UV_Sensor001");
    DEBUG_PRINT("SetModulName: ");
    DEBUG_PRINTLN(modname);

    //add image settings parameter
    String parameterImgId = eiotcloud.NewModuleParameter(moduleId, "Settings.Icon1");
    DEBUG_PRINT("parameterImgId: ");
    DEBUG_PRINTLN(parameterImgId);

    //set module image
    bool valueRet1 = eiotcloud.SetParameterValue(parameterImgId, "radiation.png");
    DEBUG_PRINT("SetParameterValue: ");
    DEBUG_PRINTLN(valueRet1);

    //now add parameter to display UV Light
    parameterId1 = eiotcloud.NewModuleParameter(moduleId, "Sensor.Parameter1");
    DEBUG_PRINT("ParameterId1: ");
    DEBUG_PRINTLN(parameterId1);

    //set parameter description
    bool valueRet2 = eiotcloud.SetParameterDescription(parameterId1, "UV");
    DEBUG_PRINT("SetParameterDescription: ");
    DEBUG_PRINTLN(valueRet2);

    //set unit
    bool valueRet3 = eiotcloud.SetParameterUnit(parameterId1, "%");
    DEBUG_PRINT("SetParameterUnit: ");
    DEBUG_PRINTLN(valueRet3);

    //Set parameter LogToDatbase
    bool valueRet4 = eiotcloud.SetParameterLogToDatabase(parameterId1, true);
    DEBUG_PRINT("SetLogToDatabase: ");
    DEBUG_PRINTLN(valueRet4);

    //SetAverageInterval  
    bool valueRet5 = eiotcloud.SetParameterAverageInterval(parameterId1, "10");
    DEBUG_PRINT("SetAverageInterval: ");
    DEBUG_PRINTLN(valueRet5);

    saveConfig();
  }

  //if something went wrong, wait here
  if (storage.moduleId == 0)
  delay(1);

  //read module ID from storage
  moduleId = String(storage.moduleId);
  //read token ID from storage
  eiotcloud.SetToken(storage.token);
  //read Sensor.Parameter1 ID from cloud
  parameterId1 = eiotcloud.GetModuleParameterByName(moduleId, "Sensor.Parameter1");
  DEBUG_PRINT("parameterId1: ");
  DEBUG_PRINTLN(parameterId1);

  Serial.println();
  Serial.println("Status\tUV (%)");
  
  lightReading = -1;
}

void loop() {
  // put your main code here, to run repeatedly:
  lightReading = analogRead(SENSOR_PIN);
  if(lightReading < MIN_VAL)
  {
    lightReading = 0;
  }
  else
  {
    lightReading = 0.05*lightReading-1;
  }
  Serial.print(lightReading);
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

