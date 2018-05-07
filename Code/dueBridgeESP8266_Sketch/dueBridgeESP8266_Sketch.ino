void setup() {
  Serial.begin(9600); //opens serial port, data rate to 9600 bps 
  Serial1.begin(9600); //opens serial port, data rate to 9600 bps
  delay(1000);
}

void loop() {
  if(Serial1.available()){
    byte b = Serial1.read(); //reads incoming bytes
    Serial.write(b); //writes incoming bytes to serial port
  }
  if(Seral.available()){
    byte c = Serial.read(); //reads incoming bytes
    Serial1.write(c); //writes incoming bytes to serial port
  }
}
