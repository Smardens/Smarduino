//const int AirValue = 785;
//const int WaterValue = 410;
//int intervals = (AirValue - WaterValue)/3;
//int soilMoistureValue = 0;
//void setup() {
//  Serial.begin(115200);
//  
//}
//
//void loop() {
//  Serial.println("Reading");
//  soilMoistureValue = analogRead(A0);
//  if(soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals))
//  {
//    Serial.println("Very Wet");
//  }
//  else if(soilMoistureValue > (WaterValue + intervals) && soilMoistureValue < (AirValue - intervals))
//  {
//    Serial.println("Wet");
//  }
//  else if(soilMoistureValue < AirValue && soilMoistureValue > (AirValue - intervals))
//  {
//    Serial.println("Dry");
//  }
//  delay(10000);
//}

void setup() {
  Serial.begin(115200); // open serial port, set the baud rate as 9600 bps
}
void loop() {
  int val;
  val = analogRead(0); //connect sensor to Analog 0
  Serial.println(val); //print the value to serial port
  delay(10000);
}
