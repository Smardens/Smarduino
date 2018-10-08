#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

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

ESP8266WebServer server(80);

void setup()
{
  Serial.begin(115200);
  Serial.println();

  setupAP();
 
}

// Handling the / root web page from my server
void handle_index() {
  server.send(200, "text/plain", "fwuack outa my server yo!");
}

// Handling the /feed page from my server
void handle_feed() {
  String id = server.arg("id");
  String t = server.arg("temp");
  String h = server.arg("hum");
  String uv = server.arg("uv");
  String sm = server.arg("soilm");

  server.send(200, "text/plain", "This is response to client");
  //StoreValues(t,h,uv,sm); create function here to store values`
}

void setupAP(){
  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP(AP_SSID, AP_PASS);
  if(result == true)
  {   
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }

  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.print(apIP); 

  setupServer();
}

void setupServer(){
  Serial.println("\n** Server Setup **");
  Serial.println("Starting server :");
  server.on("/", handle_index);
  server.on("/feed", handle_feed);
  server.begin();
};


void loop()
{
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);

  server.handleClient();
}



