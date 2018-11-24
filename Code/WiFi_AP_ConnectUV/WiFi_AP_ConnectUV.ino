/*
Proprietary Code of Smardens
The Resilient Smart Garden Project

Author:
Brian Powell @BriianPowell
v1.0
*/

//Libraries
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
//Change ID when uploading to different UV Modules
//Will also need to change static IP for each module
#define SENSORID "UV01"
//#define SENSORID "UV02" 
#define SENSOR_PIN A0 //UV sensor is on the analog input
#define MIN_VAL 550 //Minimum amount of voltage when light is extremely dark- room with no light
#define MAX_VAL 820 //Maximum amount of voltage when light is extremely bright- direct sunlight
#define INTERVAL 10 //UV scale will be on a 0 to 10 scale

//Network Config Meta Data
IPAddress ip(192,168,1,110); //Use this IPAddress for UV01
//IPAddress ip(192,168,1,111); //Use this IPAddress for UV02

IPAddress gateway(192,168,1,254);
IPAddress subnet(255,255,255,0);

//Local ESP web-server address
String serverHost = "http://192.168.1.100:80/uv";
// DEEP_SLEEP Timeout interval
int sleepInterval = 180; //Change this value for how many minutes to hibernate for: 180 is 3 hours

// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 2; //Two minutes between retry intervals
int counter = 0;

//Working Variables
String data;
int lightVal;

//Set up connection to access point and begin sensor read
void setup()
{
  ESP.eraseConfig();
  WiFi.persistent(false);
    
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(AP_SSID, AP_PASS);

  //Wait for connect
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

  //Display localIP of module
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Reading UV Sensor\n");
  readUVSensor();
  Serial.println("Constructing DATA String.\n");
  buildDataStream();
  Serial.println("Sending POST Request.\n");
  sendHttpRequest();
  Serial.println();
  Serial.println("[Sensor Reading Finished]");
  Serial.println("[Sleeping]");
  hibernate(sleepInterval);
}

//Reads value from sensor then tries to figure out what UV Value it is
void readUVSensor() {
  delay(1000);
  float scale = (MAX_VAL - MIN_VAL);
  for (int i = 0; i <= 10; i++) {
    
    if (analogVal < MIN_VAL){
      lightVal = i;
      }
    if (analogVal >= MIN_VAL &&  < ){
      lightVal = i;
      }
  }
  //Serial.println("Scale: " + String(scale));
  int analogVal = analogRead(SENSOR_PIN); // scale of voltage reading is from MAX_VAL (extremely bright) to MIN_VAL (completely dark)
  
  if (analogVal < MIN_VAL) //scale of voltage readings to UV sensor readings is distributed by a bell curve
  {
    lightVal = 0;
  }
  else if (analogVal <= MIN_VAL )//Range: 0 - 2.5; Increment by: 2.5
  {
    lightVal = 1; 
  }
  else if ( analogVal <= MIN_VAL + (scale/54) )//Range: 2.5 - 7.5; Increment by: 5 
  {
    lightVal = 2; 
  }
  else if ( analogVal <= MIN_VAL + (scale/18) )//Range: 7.5 - 22.5; Increment by: 15
  {
    lightVal = 3; 
  }
  else if ( analogVal  <= MIN_VAL + (scale/6) )//Range: 22.5 - 67.5; Increment by: 45
  {
    lightVal = 4; 
  }
  else if ( analogVal <= MIN_VAL + (scale/2) )//Range: 67.5 - 202.5; Increment by: 135
  {
    lightVal = 5; 
  }
  else if ( analogVal <= MIN_VAL + (scale/2) + (scale/6) )//Range: 202.5 - 247.5; Increment by: 45
  {
    lightVal = 6; 
  }
  else if ( analogVal <= MIN_VAL + (scale/2) +  (scale/18) )//Range: 247.5 - 262.5; Increment by: 15
  {
    lightVal = 7; 
  }
  else if (analogVal <= MIN_VAL + (scale/2) +  (scale/54) )//Range: 262.5 - 267.5; Increment by: 5
  {
    lightVal = 8; 
  }
  else if (analogVal <= MIN_VAL + (scale/2) +  (scale/216) )//Range: 267.5 - 268.75; Increment by: 1.25
  {
    lightVal = 9; 
  }
  else if ( analogVal <= MAX_VAL ) //Range: 268.75 - 270; Increment by: 1.25 
  {
    lightVal = 10; 
  }
  else {
    lightVal = 11; //above the maximum
  }

  Serial.println("Analog Value: " + String(analogVal));
  Serial.println("UV Level: "+ String(lightVal));
}
  
//Build urlencoded data stream to be sent to ESP webserver
void buildDataStream() {
  data = "id=";
  data += String(SENSORID);
  data += "&value=";
  data += String(lightVal);
  Serial.println("Data String: "+data);
}

//Utilizes HTTPClient Library to send an HTTP Post request to the webserver
//Necessary to have gateway and subnet for this to work
//Prints response code and response message
void sendHttpRequest() {
  HTTPClient http;
  http.begin(serverHost);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded", false, true);
  int httpCode = http.POST(data);
  
  Serial.println(httpCode);  
  http.writeToStream(&Serial);
  http.end();
}

//Function to hibernate the ESP module to conserve battery
//Will wake up every pInterval minutes to run setup()
void hibernate(int pInterval) {
  WiFi.disconnect();
  ESP.deepSleep(10 * 600000 * pInterval, WAKE_RFCAL);
  delay(100);
}

void loop() {}
