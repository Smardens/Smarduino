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
#define WiFi_USERNAME "BrokeBois" //change
#define WiFi_PASSWORD "S1gma3tabetaCh1" //change
#define INSTANCE_ID "5b8ef14047976c47a0d13172"
#define TOKEN "eXsNNpCfLl9T55eRh2pC77A5CsQXlZSbTL5XPWTx"

#define CONFIG_START 32
#define CONFIG_VERSION "v01"

#define REPORT_INTERVAL 60 //in seconds

#define MIN_VAL 20

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
  "eXsNNpCfLl9T55eRh2pC77A5CsQXlZSbTL5XPWTx",
  //the default module 0 - invalid module
  0,
  //0//not valid
};

float lightVal;

String moduleId = "";
String parameterId1 = "";

void setup() {
  Serial.begin(115200);
  DPRINTLN("Start...");

  EEPROM.begin(512);
  loadConfig();

  eiotcloud.begin(WiFi_USERNAME, WiFi_PASSWORD, TOKEN);
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

    //get new token - alternarive is to manually create token and store it in EEPROM
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
    bool modname = eiotcloud.SetModulName(moduleId, "UV_01");
    DPRINT("SetModulName: ");
    DPRINTLN(modname);

    //add image settings parameter
    String parameterImgId = eiotcloud.NewModuleParameter(moduleId, "Settings.Icon1");
    DPRINT("parameterImgId: ");
    DPRINTLN(parameterImgId);

    //set module image
    bool valueRet1 = eiotcloud.SetParameterValue(parameterImgId, "radiation.png");
    DPRINT("SetParameterValue: ");
    DPRINTLN(valueRet1);

    //now add parameter to display UV Light
    parameterId1 = eiotcloud.NewModuleParameter(moduleId, "Sensor.Parameter1");
    DPRINT("ParameterId1: ");
    DPRINTLN(parameterId1);

    //set parameter description
    bool valueRet2 = eiotcloud.SetParameterDescription(parameterId1, "UV");
    DPRINT("SetParameterDescription: ");
    DPRINTLN(valueRet2);

    //set unit
    bool valueRet3 = eiotcloud.SetParameterUnit(parameterId1, "INDEX");
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
  Serial.println("Status\nUV (Index)");
}

void loop() {
  int analogVal = analogRead(SENSOR_PIN);
  if (analogVal < MIN_VAL)
  {
    lightVal = 0;
  }
  else
  {
    lightVal = 0.05 * analogVal - 1;
  }
  Serial.println();
  Serial.println(lightVal);

  eiotcloud.SetParameterValue(parameterId1,String(lightVal));

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

