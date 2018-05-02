const int AirValue = 785;
const int WaterValue = 391;
int intervals = (AirValue - WaterValue)/3;
int soilMoistureValue = 0;
void setup() {
  Serial.begin(9600);

}

void loop() {
  soilMoistureValue = analogRead(A0);
  if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
  {
    Serial.println("Very Wet");
  }
  else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
  {
    Serial.println("Wet");
  }
  else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
  {
    Serial.println("Dry");
  }
  delay(100);
}
