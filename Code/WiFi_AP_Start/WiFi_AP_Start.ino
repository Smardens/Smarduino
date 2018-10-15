#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>

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

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  Serial.println();

  setupAP();
}

void setupAP() {
  Serial.print("Setting soft-AP ... ");
  WiFi.disconnect();
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

// Handling the / root web page from my server
void handle_index() {
  server.send(200, "text/plain", "fwuack outa my server yo!");
}

// Handling the /uv page from my server
void handle_uv() {
  String id = server.arg("id");
  String value = server.arg("value");

  server.send(200, "text/plain", "This is a positive resposne to the client.");
  Serial.println("Sensor ID: " + String(id) + "\nSensor Value: " + String(value));
}

// Handling the /sm page from my server
void handle_sm() {
  String id = server.arg("id");
  String value = server.arg("value");

  server.send(200, "text/plain", "This is a positive resposne to the client.");
  Serial.println("Sensor ID: " + String(id) + "\nSensor Value: " + String(value));
}

// Handling the /dht page from my server
void handle_dht() {
  String id = server.arg("id");
  String tVal = server.arg("temp");
  String hVal = server.arg("hum");

  server.send(200, "text/plain", "This is a positive resposne to the client.");
  Serial.println("Sensor ID: " + String(id) + "\nTemperature Value: " + String(tVal) + "\nHumidity Value: " + String(hVal));
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

void loop() {
  server.handleClient();
}

