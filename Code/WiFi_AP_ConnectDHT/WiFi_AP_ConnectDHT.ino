#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

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
//Change ID when uploading to different DHT Modules
//Will also need to change static IP for each module
#define SENSORID "HT01"
//#define SENSORID "HT02"
#define DHTPIN 4

//Network Config Meta Data
IPAddress ip(192,168,1,120); //Use this IPAddress for DHT01
//IPAddress ip(192,168,1,121); //Use this IPAddress for DHT02

IPAddress gateway(192,168,1,254);
IPAddress subnet(255,255,255,0);

//Local ESP web-server address
String serverHost = "http://192.168.1.100:80/dht";
// DEEP_SLEEP Timeout interval
int sleepInterval = 180; //Change this value for how many minutes to hibernate for: 180 is 3 hours

// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 2; //Two minutes between retry intervals
int counter = 0;

//Working Variables
String data;
float hum;
float temp;

DHT dht;


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
    if (counter > 20) {
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

  dht.setup(DHTPIN);

  Serial.println("Reading DHT Sensor\n");
  readDHTSensor();
  Serial.println("Constructing DATA String.\n");
  buildDataStream();
  Serial.println("Sending POST Request.\n");
  sendHttpRequest();
  Serial.println();
  Serial.println("[Sensor Reading Finished]");
  Serial.println("[Sleeping]");
  hibernate(sleepInterval);
}

void readDHTSensor() {
  delay(1000);
  hum = dht.getHumidity();
  temp = dht.getTemperature();
  if (isnan(hum) || isnan(temp)) {
    temp = 0.00;
    hum = 0.00;
  }
  Serial.println("Temperature Read: " + String(temp));
  Serial.println("Humidity Read: " + String(hum));
}

void buildDataStream() {
  data = "id=";
  data += String(SENSORID);
  data += "&temp=";
  data += String(temp);
  data += "&hum=";
  data += String(hum);
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
  ESP.deepSleep(10 * 600000 * pInterval, WAKE_RFCAL);
  delay(100);
}

void loop() {}
