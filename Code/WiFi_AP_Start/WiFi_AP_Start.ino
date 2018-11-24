/*
Proprietary Code of Smardens
The Resilient Smart Garden Project

Author:
Brian Powell @BriianPowell
v1.0
*/

//Libraries
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

//Network Configuration Meta Data
IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

//Access Point Credentials
#define AP_SSID "Smardens_AP"
#define AP_PASS "FluffyBunny69"

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println();

  setupAP();
}

//Set up Soft Access Point off the ESP module to transmist data to
void setupAP() {
  Serial.print("Setting soft-AP ... ");

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, gateway, subnet);
  boolean result = WiFi.softAP(AP_SSID, AP_PASS);
  if (result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }

  IPAddress apIP = WiFi.softAPIP();
  Serial.print("\nAP IP address: ");
  Serial.print(apIP);

  setupServer();
}

//Create HTTP endpoints for HTTP Post handling
void setupServer() {
  Serial.println("\n[Server Setup]");
  Serial.println("[Starting server]");
  server.on("/", handle_index);
  server.on("/uv", handle_uv);
  server.on("/sm", handle_sm);
  server.on("/dht", handle_dht);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("[Server Listening]");
};

//Handling the / root web page from my server
void handle_index() {
  server.send(200, "text/plain", "ERROR! You don't have access to this server.");
}

//Handling the /uv page from my server
void handle_uv() {
  String id = server.arg("id");
  String value = server.arg("value");

  server.send(200, "text/plain", "Message received from client.");
  Serial.println("Sensor ID: " + String(id) + "   |   Sensor Value: " + String(value));
}

//Handling the /sm page from my server
void handle_sm() {
  String id = server.arg("id");
  String value = server.arg("value");

  server.send(200, "text/plain", "Message received from client.");
  Serial.println("Sensor ID: " + String(id) + "   |   Sensor Value: " + String(value));
}

//Handling the /dht page from my serverdht
void handle_dht() {
  String id = server.arg("id");
  String tVal = server.arg("temp");
  String hVal = server.arg("hum");

  server.send(200, "text/plain", "Message received from client.");
  Serial.println("Sensor ID: " + String(id) + "   |   Temperature Value: " + String(tVal) + " Humidity Value: " + String(hVal));
}

//Handle not found server page and redirect to 404
void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

//Constantly check for CRUD Http methods
void loop() {
  server.handleClient();
}

