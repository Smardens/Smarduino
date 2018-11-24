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

//Static Network Config
IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192,168,1,254);
IPAddress subnet(255,255,255,0);

//Local ESP web-server address
String serverHost = "http://192.168.1.100:80/uv";
// DEEP_SLEEP Timeout interval
int sleepInterval = 180; //Change this value for how many minutes to hibernate for: 180 is 3 hours

// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 2; //Two minutes between retry intervals
int counter = 0;


//Set up connection to access point and begin sensor read
void setup()
{
  ESP.eraseConfig();
  WiFi.persistent(false);
    
  Serial.begin(115200);
  Serial.println();

  //Setup WiFi configs and connect
  WiFi.mode(WIFI_STA)
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(AP_SSID, AP_PASS);

  //Wait for connect
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    if(counter > 20){
       Serial.println("Can't Find AP.\n [Sleeping]");    
       hibernate(failConnectRetryInterval); //If connect fails hibernate and try connect again in two minutes
    }
    delay(500);
    Serial.print(".");
    counter++;
  }
  Serial.println();

  //Display localIP of module
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

//Function to hibernate the ESP module to conserve battery
//Will wake up every pInterval minutes to run setup()
void hibernate(int pInterval) {
  WiFi.disconnect();
  ESP.deepSleep(10 * 600000 * pInterval, WAKE_RFCAL);
  delay(100);
}

void loop() {}
