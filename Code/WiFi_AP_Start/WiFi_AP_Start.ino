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

//Access Point Credentials
#define AP_SSID "Smardens_AP"
#define AP_PASS "FluffyBunny69"

//Post Request values
String id;
String temp;
String hum;
String uv; 
String sm;

//initialize variable
int solenoidPin = 2;
int duration1=1000*60*5; //5 min
int duration2=1000*60*2.5; //2.5 min
int dryness=0;
int dryness2=0;
bool waterTwice = false; //default, water only once per day

ESP8266WebServer server(80);

void setup()
{
  pinMode(solenoidPin, OUTPUT); 
  setTime(00,00,00,9,10,2018); // 10-9-2018 00:00:00
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
  id = server.arg("id");
  temp = server.arg("temp");
  hum = server.arg("hum");
  uv = server.arg("uv");
  sm = server.arg("sm");

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
  if(WiFi.softAPgetStationNum() >= 1){
    Serial.println(WiFi.softAPgetStationNum());
  }

  server.handleClient();

  int currentDuration=0;
  //5 minute before normal watering schedule, check the soil moisture level to determine length of watering
  if(hour()==7 && minute()==55 && second==00)
  {
     String sm1[2],sm2[2],sm3[2],sm4[2],sm5[2];
     //READ SOIL MOISTURE FROM SD 
     //sm1[]=read whatever etc. 
      int m1[2],m2[2],m3[2],m4[2],m5[2];
      
      //convert moisture level to int
      for (int i = 0; i<2;i++)
      {
        m1[i]=sm1[i].toInt();
        m2[i]=sm2[i].toInt();
        m3[i]=sm3[i].toInt();
        m4[i]=sm4[i].toInt();
        m5[i]=sm5[i].toInt();
      }
      
     //check last soil moisture level
     dryness = (m1[0]+m2[0]+m3[0]+m4[0]+m5[0])/5; //average of 5 soil moisture level
     dryness2 = (m1[1]+m2[1]+m3[1]+m4[1]+m5[1])/5 //average of 5 soil mositure level from second to last data
     
    //check last soil moisture level
    String check;
    if(checkDry(dryness)=="Dry" && checkDry(dryness2)=="Dry")
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
       waterTwice = false;
       currentDuration = duration2;
     }
     else
     {
       waterTwice = false;
       currentDuration = 0; //don't open if the field is super wet
     }
  }
  time1(currentDuration);



  if(hour()==13 && minute()==0 && second==0)
  {
    //READ MOISTURE HERE
    dryness=(m1+m2+m3+m4+m5)/5;
    //int h1 = //READ HUMIDITY FROM ARDUINO
   // int h2 = //READ FROM SD UV
    //int t1 = //READ HUMIDITY FROM ARDUINO
    //int t2 = //READ FROM SD TEMP
    //int u1 = //READ UV FROM ARDUINO
    //int u2 = //READ FROM SD UV
    
    int humi = (h1+h2)/2;
    int temp = (t1+t2)/2;
    int uv = (u1+u2)/2;
    
    if(humi>70 && temp<15 && uv<2  && check(dryness)=="Dry")
    {
      cuurentDuration = duration2;
    }
    else if (humi>70 && temp<15 && uv<2  && check(dryness)=="Wet" || check(dryness)=="Very Wet")
    {
      currentDuration = 0;
    }
    else 
    if(temp>36)
    {
      waterTwice==true;
    }
  }

  // before second watering
  if(waterTwice==true && hour()==15 && minute()==55 && second==0)
   {
    
      String sm1,sm2,sm,sm4,sm5;
      //READ SOIL MOSITURE SD 
      //sm1=
      //sm2= 
      //etc.
      int m1,m2,m3,m4,m5;
      m1=sm1.toInt();
      m2=sm2.toInt();
      m3=sm3.toInt();
      m4=sm4.toInt();
      m5=sm5.toInt();

      //check last soil moisture level
      dryness = (m1+m2+m3+m4+m5)/5; //average of 5 soil moisture level
      String check = checkDry(dryness);
      if(check=="Dry")
      {
        currentDuration=duration2;
      }
      else if(check=="Wet")
      {
        currentDuration = 0;
      }
   }

   if(hour()==23 && minute()==55 && second==0)
   {
     waterTwice=false;
   }
}
// //normal watering schedule with normal water amount
//void time1(int duration)
//{
//    if(hour()==8 && minute()==0 && second()==0)
//  {
//     digitalWrite(solenoidPin, HIGH); //open valve
//     delay(duration) 
//     digitalWrite(solenoidPin, LOW); //close valve
//  }
//}
//
////second watering schedule if needed
//void time2(int duration)
//{
//  if(hour()==16 && minute()==0 && second()==0)
//  {
//     digitalWrite(solenoidPin, HIGH); //open valve
//     delay(duration) 
//     digitalWrite(solenoidPin, LOW); //close valve
//  }
//}
//
////Moisture level check as dry, wet, super wet
//String dryness()
//{
//  const int AirValue = 785;
//  const int WaterValue = 391;
//  int intervals = (AirValue - WaterValue)/3;
//  int soilMoistureValue = 0;
//  if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
//  {
//    return "Very Wet";
//  }
//  else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
//  {
//    return "Wet";
//  }
//  else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
//  {
//    return "Dry";
//  }
//}

