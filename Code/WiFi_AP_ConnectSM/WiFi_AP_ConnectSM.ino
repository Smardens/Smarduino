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
//Change ID when uploading to different Soil Moisture Modules
//Will also need to change static IP for each module
#define SENSORID "SM01"
//#define SENSORID "SM02"
//#define SENSORID "SM03"
//#define SENSORID "SM04"
//#define SENSORID "SM05"
#define SENSOR_PIN A0 //Soil Moisture sensor is on the analog input

//Network Config Meta Data
IPAddress ip(192,168,1,101); //Use this IPAddress for SM01
//IPAddress ip(192,168,1,102); //Use this IPAddress for SM02
//IPAddress ip(192,168,1,103); //Use this IPAddress for SM03
//IPAddress ip(192,168,1,104); //Use this IPAddress for SM04
//IPAddress ip(192,168,1,105); //Use this IPAddress for SM05

IPAddress gateway(192,168,1,254);
IPAddress subnet(255,255,255,0);

//Local ESP web-server address
String serverHost = "http://192.168.1.100:80/sm";
// DEEP_SLEEP Timeout interval
int sleepInterval = 180; //Change this value for how many minutes to hibernate for: 180 is 3 hours

// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 2; //Two minutes between retry intervals
int counter = 0;

//Working Variables
String data;
int soilMoistureValue = 0;


void setup()
{
  ESP.eraseConfig();
  WiFi.persistent(false);
  
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA);
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
  Serial.println("Constructing DATA String.\n");
  buildDataStream();
  Serial.println("Sending POST Request.\n");
  sendHttpRequest();
  Serial.println();
  Serial.println("[Sensor Reading Finished]");
  Serial.println("[Sleeping]");
  hibernate(sleepInterval);
}

void readSMSensor() {
  delay(1000);
  soilMoistureValue = analogRead(SENSOR_PIN);  //put Sensor insert into soil
  Serial.println("SM Level: " + String(soilMoistureValue));
}

void buildDataStream() {
  data = "id=";
  data += String(SENSORID);
  data += "&value=";
  data += String(soilMoistureValue);
  Serial.println("Data String: " + data);
}
  
void sendHttpRequest() {
  HTTPClient http;
  http.begin(serverHost);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded", false, true);
  int httpCode = http.POST(data);
  
  Serial.println(httpCode);  
  http.writeToStream(&Serial);
  http.end();
}

void hibernate(int pInterval) {
  WiFi.disconnect();
  ESP.deepSleep(60 * 1000000 * pInterval, WAKE_RFCAL); 
  delay(1000);
}

void loop() {}
