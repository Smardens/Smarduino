/*#This Sample code is for testing the UV Sensor
#Connection:
  VCC-5V
  GND-GND
  OUT-Analog pin 0 
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //open serial port, set baud rate to 9600 bps  
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue;
  int analogValue = analogRead(0);
  if(analogValue<20){
    sensorValue = 0;
  }
  else{
    sensorValue = 0.05 * analogValue-1;
  }
  Serial.println(sensorValue); //print the value to serial
  delay(200);
}
