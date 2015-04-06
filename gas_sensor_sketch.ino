#include <Time.h>
#include <SD.h>
#include <SPI.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message

const int chipSelect = 8;

void setup() {
  // Set SS pin as output for SD card
  pinMode(chipSelect, OUTPUT);
  // Setup sensors
  pinMode(A4, INPUT);
  pinMode(A3, INPUT);
  pinMode(A2, INPUT);
  pinMode(A1, INPUT);
  pinMode(A0, INPUT);
  
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  Serial.println("Waiting for sync message");
  
}



void loop() {
  String sensors[5] = { "MQ7(CO): ", ",MQ8(H2): ", ",MQ6(LPG): ", ",MQ3(Alcohol): ", ",MQ4(CH4): "};
  String dataString = "";
  if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus()!= timeNotSet) {
    digitalClockDisplay();  
  }
  if (timeStatus() == timeSet) {
    digitalWrite(13, HIGH); // LED on if synced
  // read three sensors and append to the string:
    for (int analogPin = 0; analogPin < 5; analogPin++) {
      int sensor = analogRead(analogPin);
      dataString = dataString + sensors[analogPin] + String(sensor);
      if (analogPin < 2) {
      }
    }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  //  bool dataFile = true;
  // if the file is available, write to it:
    if (dataFile) {
      digitalClockRecord(dataFile);
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println(dataString);
      digitalWrite(13, HIGH);
      delay(200);
      digitalWrite(13, LOW);
      delay(200);
      digitalWrite(13, HIGH);
      delay(200);
      digitalWrite(13, LOW);
      delay(200);
    } else {
      Serial.println("error opening datalog.txt");
    }
  } else {
    digitalWrite(13, LOW);  // LED off if needs refresh
  }
  delay(30000);
  
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.print(": "); 
}

void digitalClockRecord(File file) {
  file.print(hour());
  recordDigits(minute(), file);
  recordDigits(second(), file);
  file.print(" ");
  file.print(day());
  file.print(" ");
  file.print(month());
  file.print(" ");
  file.print(year());
  file.print(": "); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void recordDigits(int digits, File file) {
  file.print(":");
  if(digits < 10)
    file.print('0');
  file.print(digits);
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

