#include <ESP8266WiFi.h>
#include "EIoTCloudRestApiV1.0.h"
#include <EEPROM.h>

#define DEBUG

#ifdef DEBUG
#define DPRINTLN(x)  Serial.println(x)
#define DPRINT(x)    Serial.print(x)
#else
#define DPRINTLN(x)
#define DPRINT(x)
#endif


EIoTCloudRestApi eiotcloud;

//change lines for wifi name and password
#define WiFi_USERNAME "" //change
#define WiFi_PASSWORD "" //change
#define INSTANCE_ID "5b8ef14047976c47a0d13172"
#define TOKEN "xEXI4oJGJwaHedqbe1206iP8RWEeA6tnWUWa4YD0"

#define REPORT_INTERVAL 60 //in seconds

#define CONFIG_START 32
#define CONFIG_VERSION "v01"

#define AIR_VAL 520
#define WATER_VAL 260

#define SENSOR_PIN A0//uv sensor is on the analog input

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
  "xEXI4oJGJwaHedqbe1206iP8RWEeA6tnWUWa4YD0",
  //the default module 0 - invalid module
  0,
};

int intervals = (AIR_VAL - WATER_VAL) / 3;

int soilMoistureValue = 0;
String soilMoistureLevel = "";

String moduleId = "";
String parameterId1 = "";

void setup() {
  Serial.begin(115200);
  DPRINTLN("Start...");

  EEPROM.begin(512);
  loadConfig();

  eiotcloud.begin(WiFi_USERNAME, WiFi_PASSWORD);
  //eiotcloud.SetToken(TOKEN);
  
  Serial.println("Connecting to ");
  Serial.print(WiFi_USERNAME); Serial.println("...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.print('\n');
  Serial.println("Connection Established");
  Serial.print("IP Address:\t");
  Serial.println(WiFi.localIP());
  Serial.println("ModuleId:");
  Serial.println(storage.moduleId);


  //if first time get new token and register new module
  //here happened Plug and Play logic to add module to cloud
  if (storage.moduleId == 0)
  {
    //get new token - alternative is to manually create token and store it in eeprom
    String token = eiotcloud.TokenNew(INSTANCE_ID);
    DPRINT("Token: ");
    DPRINTLN(token);
    eiotcloud.SetToken(token);

    //remember token
    token.toCharArray(storage.token, 41);

    //add new module and configure it
    moduleId = eiotcloud.ModuleNew();
    DPRINT("ModuleId: ");
    DPRINTLN(moduleId);
    storage.moduleId = moduleId.toInt();

    // set module type
    bool modtyperet = eiotcloud.SetModulType(moduleId, "MT_GENERIC");
    DPRINT("SetModulType: ");
    DPRINTLN(modtyperet);

    // set module name
    bool modname = eiotcloud.SetModulName(moduleId, "SoilMoisture_01");
    DPRINT("SetModulName: ");
    DPRINTLN(modname);

    //add image settings parameter
    String parameterImgId = eiotcloud.NewModuleParameter(moduleId, "Settings.Icon1");
    DPRINT("parameterImgId: ");
    DPRINTLN(parameterImgId);

    //set module image
    bool valueRet1 = eiotcloud.SetParameterValue(parameterImgId, "ao.png");
    DPRINT("SetParameterValue: ");
    DPRINTLN(valueRet1);

    //now add parameter to display UV Light
    parameterId1 = eiotcloud.NewModuleParameter(moduleId, "Sensor.Parameter1");
    DPRINT("ParameterId1: ");
    DPRINTLN(parameterId1);

    //set parameter description
    bool valueRet2 = eiotcloud.SetParameterDescription(parameterId1, "Moisture Level");
    DPRINT("SetParameterDescription: ");
    DPRINTLN(valueRet2);

    //set unit
    bool valueRet3 = eiotcloud.SetParameterUnit(parameterId1, "Moisture Level");
    DPRINT("SetParameterUnit: ");
    DPRINTLN(valueRet3);

    //Set parameter LogToDatbase
    bool valueRet4 = eiotcloud.SetParameterLogToDatabase(parameterId1, true);
    DPRINT("SetLogToDatabase: ");
    DPRINTLN(valueRet4);

    //SetAverageInterval
    bool valueRet5 = eiotcloud.SetParameterAverageInterval(parameterId1, "10");
    DPRINT("SetAverageInterval: ");
    DPRINTLN(valueRet5);

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
  DPRINT("parameterId1: ");
  DPRINTLN(parameterId1);

  Serial.println();
  Serial.println("Status\tMoisture Level");
}

void loop() {
  // put your main code here, to run repeatedly:

  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  if (soilMoistureValue > WATER_VAL && soilMoistureValue < (WATER_VAL + intervals))
  {
    soilMoistureLevel = "Very Wet";
  }
  else if (soilMoistureValue > (WATER_VAL + intervals) && soilMoistureValue < (AIR_VAL - intervals))
  {
    soilMoistureLevel = "Wet";
  }
  else if (soilMoistureValue < AIR_VAL && soilMoistureValue > (AIR_VAL - intervals))
  {
    soilMoistureLevel = "Dry";
  }

  bool valueRet = eiotcloud.SetParameterValue(parameterId1, String(soilMoistureLevel));
  DPRINT("\nSetParameterValue: ");
  DPRINTLN(valueRet);

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

