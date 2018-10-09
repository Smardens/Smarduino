C#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DEBUG
#ifdef DEBUG
#define DPRINTLN(x)  Serial.println(x)
#define DPRINT(x)    Serial.print(x)
#else
#define DPRINTLN(x)
#define DPRINT(x)
#endif

#define AP_SSID "Smardens_AP"
#define AP_PASS "FluffyBunny69"

//Local ESP web-server address
String serverHost = "http://192.168.4.1/feed";
String data;
// DEEP_SLEEP Timeout interval
int sleepInterval = 5;

// DEEP_SLEEP Timeout interval when connecting to AP fails
int failConnectRetryInterval = 2;
int counter = 0;

//Staic Network Configuration
IPAddress ip(192, 168, 4, 4);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255, 255, 255, 0);

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(AP_SSID, AP_PASS);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {}
