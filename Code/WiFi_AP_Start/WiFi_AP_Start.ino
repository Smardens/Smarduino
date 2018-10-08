#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <TimeLib.h>

//Debug Macros
#define DEBUG
#ifdef DEBUG
#define DPRINTLN(x)  Serial.println(x)
#define DPRINT(x)    Serial.print(x)
#else
#define DPRINTLN(x)
#define DPRINT(x)
#endif

//initialize variable
int solenoidPin = 2;
int count = 0; //how many dry is received
int duration1=1000*60*5; //5 min
int duration2=1000*60*2.5 //2.5 min
bool waterTwice = false; //default, water only once per day

//Access Point Credentials
#define AP_SSID "Smardens_AP"
#define AP_PASS "FluffyBunny69"

ESP8266WebServer server(80);

void setup()
{
  pinMode(solenoidPin, OUTPUT); 
  setTime(00,00,00,09,10,2018); // 10-9-2018 00:00:00
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

  int currentDuration=0;
  //5 minute before normal watering schedule, check the soil moisture level to determine length of watering
  if(hour()==7 && minute()==55 && second==00)
  {
    //check last soil moisture level
    dryness = (m1[0]+m2[0]+m3[0]+m4[0]+m5[0])/5; //average of 5 soil moisture level
    dryness2 = (m1[1]+m2[1]+m3[1]+m4[1]+m5[1])/5 //average of 5 soil mositure level from second to last data
    if(checkDry(dryness)=="Dry" && checkDry(dryness)=="Dry")
    {
      waterTwice=true;
    }
    String check = checkDry(dryness);
    if(check=="Dry")
    {
      currentDuration=duration1;
    }
    else if(check=="Wet")
    {
      currentDuration = duration2;
    }
    else
    {
      currentDuration = 0; //don't open if the field is super wet
    }

  }
   time1(currentDuration);
  
   if(waterTwice==true && hour()==15 && minute()==55 && second==0)
   {
      //check last soil moisture level
      dryness = (m1[0]+m2[0]+m3[0]+m4[0]+m5[0])/5; //average of 5 soil moisture level
      String check = checkDry(dryness);
      if(check=="Dry")
      {
        currentDuration=duration1;
      }
      else if(check=="Wet")
      {
        currentDuration = duration2;
      }
      else
      {
        currentDuration = 0; //don't open if the field is super wet
      }
   }

   if(hour()==23 && minute()==55 && second==0)
   {
     waterTwice=false;
   }
}

 //normal watering schedule with normal water amount

void time1(duration)
{
    if(hour()==8 && minute()==0 && second()==0)
  {
     digitalWrite(solenoidPin, HIGH); //open valve
     delay(duration) 
     digitalWrite(solenoidPin, LOW); //close valve
  }
}

//second watering schedule if needed
void time2(int duration)
{
  if(hour()==16 && minute()==0 && second()==0)
  {
     digitalWrite(solenoidPin, HIGH); //open valve
     delay(duration) 
     digitalWrite(solenoidPin, LOW); //close valve
  }
}

//Moisture level check as dry, wet, super wet
String dryness(int dryness)
{
  const int AirValue = 785;
  const int WaterValue = 391;
  int intervals = (AirValue - WaterValue)/3;
  int soilMoistureValue = 0;
  if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
  {
    return "Very Wet";
  }
  else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
  {
    return "Wet";
  }
  else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
  {
    return "Dry";
  }
}

