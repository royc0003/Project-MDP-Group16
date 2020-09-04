int piTest = 0;
//int ledTest = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // initialize the baud rate
  //pinMode(ledTest, OUTPUT);
  Serial.println("Starting Communication!");
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0)
  {
    piTest = Serial.read();
    Serial.print("I received: ");
    Serial.write(piTest);
    Serial.println();

    /*
    if(piTest = 'S')
    {
      digitalWrite(ledTest, HIGH);
      delay(2000);
      digitalWrite(ledTest, LOW);
      delay(2000);
    }
    */
  }
  delay(2000);
}
