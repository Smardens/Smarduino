#include <TimeLib.h>
int solenoidPin = 2;
int duration1=1000*60*5; //5 min
int duration2=1000*60*2.5 //2.5 min
int sm1 = 0;
int sm2 = 0;

void setup() {
  // pinMode(solenoidPin, OUTPUT); 
  Serial.begin(9600);
  setTime(00,00,00,09,10,2018); // 10-9-2018 00:00:00
}

void loop() 
{
  int currentDuration=0;
  //5 minute before normal watering schedule, check the soil moisture level to determine length of watering
  if(hour()==7 && minute()==55 && second==00)
  {
     s1 = analogRead(A0).toInt();//soil moisture to A0
     s2 = analogRead(A1).toInt();//soil moisture to A1
     dryness = (s1+s2)/2; //average of 5 soil moisture level
     
    //check last soil moisture level
    String check;
    if(checkDry(dryness)=="Dry")
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


  // second watering check time
  if(hour()==15 && minute()==55 && second==0)
   {
      sm1 = analogRead(A0).toInt();
      sm2 = analogRead(A1).toInt();

      //check last soil moisture level
      dryness = (sm1+sm2)/2; //average of 5 soil moisture level
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
  time2(currentDuration);
 
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
     delay(duration) //1000 = 1 second
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


