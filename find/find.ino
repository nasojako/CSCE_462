/*
 * App to locate stick by sounding alarm over Bluetooth
 * Jason Robles
 */

#include <CurieBLE.h>

const int trigPin = 12;
const int echoPin = 11;
const int alarm = 9;
const int controlAlarm = 10;
int val = 0;

BLEPeripheral blePeripheral; // create peripheral instance
BLEService alarmControl("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service


// create switch characteristic and allow remote device to read and write
BLECharCharacteristic alarmStatus("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  // set alarm and control for alarm as output pins
  pinMode(alarm, OUTPUT);
  pinMode(controlAlarm, OUTPUT);

  // set the local name peripheral advertises
  blePeripheral.setLocalName("Alarm");
  // set the UUID for the service this peripheral advertises:
  blePeripheral.setAdvertisedServiceUuid(alarmControl.uuid());

  // add service and characteristics
  blePeripheral.addAttribute(alarmControl);
  blePeripheral.addAttribute(alarmStatus);

  // set initial value for the characteristic
  alarmStatus.setValue(0);

  // advertise the service
  blePeripheral.begin();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // block of code to control ultrasonic sensing
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
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

  if(inches <= 10) {
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


  //block of code to control bluetooth application
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the alarm:
      if (alarmStatus.written()) {
        val = alarmStatus.value();
        Serial.println("value: ");
        Serial.println(val);
        if (val == 49) {   // 49 is '1' in ASCII
          Serial.println("Alarm on");
          digitalWrite(controlAlarm, LOW); // make sure it gets a clean HIGH
          digitalWrite(controlAlarm, HIGH);
          tone(alarm, 2600); 
        } else {                             
          Serial.println("Alarm off");
          noTone(alarm);
          digitalWrite(controlAlarm, LOW);          // will turn the alarm off
        }
      }
    }
    //TO DO create graceful disconnect
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

// functions to convert distances
long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}


