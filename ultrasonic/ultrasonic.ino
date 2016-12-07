//// ---------------------------------------------------------------------------
//// Jason Robles
//// CSCE_462 502
//// ---------------------------------------------------------------------------
//

// this constant won't change.  It's the pin number
// of the sensor's output:
const int trigPin = 12;
const int echoPin = 11;
const int alarm = 9;
const int controlAlarm = 10;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  pinMode(alarm, OUTPUT);
  pinMode(controlAlarm, OUTPUT);
}

void loop() {
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  delay(100);

  if(cm <= 10) {
    digitalWrite(controlAlarm, LOW); // make sure it gets a clean HIGH
    digitalWrite(controlAlarm, HIGH);
    
    for(int i=0;i<10;i++) {
      tone(alarm, 2600);
      delay(100); // alarm for 3 seconds
      noTone(alarm); // stop alarm
      delay(100);
    }
    digitalWrite(controlAlarm, LOW);
  }
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
