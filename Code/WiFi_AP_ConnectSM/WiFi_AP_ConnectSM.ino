#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//Debug Macros
#define DEBUG
#ifdef DEBUG
#define DPRINTLN(x)  Serial.println(x)
#define DPRINT(x)    Serial.print(x)
#else
#define DPRINTLN(x)
#define DPRINT(x)
#endif

//Access Point Credentials
#define AP_SSID "Smardens_AP"
#define AP_PASS "FluffyBunny69"

//Sensor Meta Data
#define SENSORID "SM01" //change this id when uploading to different sensor modules
#define SENSOR_PIN A0 //Soil Moisture sensor is on the analog input
#define AIR_VAL 520
#define WATER_VAL 260

//Local ESP web-server address
String serverHost = "http://192.168.4.1/feed";
String data;
// DEEP_SLEEP Timeout interval
int sleepInterval = 5;

// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 2;
int counter = 0;

//Working Variables
int intervals = (AIR_VAL - WATER_VAL) / 3;
int soilMoistureValue = 0;
String soilMoistureLevel = "";

//Staic Network Configuration
IPAddress ip(192, 168, 4, 4);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiClient client;

void setup()
{
  ESP.eraseConfig();
  WiFi.persistent(false);
  
  Serial.begin(115200);
  Serial.println();

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(AP_SSID, AP_PASS);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    if(counter > 20){
       Serial.println("Can't Find AP.\n [Sleeping]");    
       hibernate(failConnectRetryInterval);
    }
    delay(500);
    Serial.print(".");
    counter++;
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());


  Serial.println("Reading Soil Moisture Sensor\n");
  readSMSensor();
  Serial.println("Construct DATA String.\n");
  buildDataStream();
  Serial.println("Sending Get Request.\n");
  sendHttpRequest();
  Serial.println();
  Serial.println("[Sensor Reading Finished]");
  Serial.println("[Sleeping]");
  hibernate(sleepInterval);
}

void readSMSensor() {
  soilMoistureValue = analogRead(SENSOR_PIN);  //put Sensor insert into soil
  if (soilMoistureValue > WATER_VAL && soilMoistureValue < (WATER_VAL + intervals))
  {
    soilMoistureLevel = "Very+Wet";
  }
  else if (soilMoistureValue > (WATER_VAL + intervals) && soilMoistureValue < (AIR_VAL - intervals))
  {
    soilMoistureLevel = "Wet";
  }
  else if (soilMoistureValue < AIR_VAL && soilMoistureValue > (AIR_VAL - intervals))
  {
    soilMoistureLevel = "Dry";
  }

  Serial.println("Soil Moisture Level: " + String(soilMoistureLevel));
}

void buildDataStream() {
  data = "id=";
  data += String(SENSORID);
  data += "&temp=";
  data += String(0);
  data += "&hum=";
  data += String(0);
  data += "&uv=";
  data += String(0);
  data += "&sm=";
  data += String(soilMoistureLevel);
  Serial.println("Data Stream: " + data);
}

void sendHttpRequest() {
  HTTPClient http;
  http.begin(serverHost);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(data);
  http.writeToStream(&Serial);
  http.end();
}

void hibernate(int pInterval) {
  WiFi.disconnect();
  ESP.deepSleep(10 * 600000 * pInterval, WAKE_RFCAL);
  delay(100);
}

void loop() {}