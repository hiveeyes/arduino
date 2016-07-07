/*
  MonitorDMS
  Reads an analog input using a ADS1231, converts it to strain, and prints the result to the serial monitor.
 
 This example code is in the public domain.
 */

 #include <ADS1231.h>

 ADS1231 myDMS;  // create ADS1231 object to monitor a DMS  

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  // attach the DMS on pin 15 SCL, 5 Data, 19 PowerDown
  myDMS.attach( 15 , 5 , 19 );
  //
  Serial.println("ADS1231");
  Serial.println("time [ms] , sensor , dt sensor read [us]");
}

// the loop routine runs over and over again forever:
void loop() {
  // check for data ready
  if( myDMS.check() ) {
    // get start time of read
    unsigned long timeStart = micros();
    // read the input on ADS1231:
    long sensorValue = myDMS.readData();
    // get final time of read
    unsigned long timeEnd = micros();
    // print out the time, the value read and time required for read:
    Serial.print(millis());
    Serial.print(" , ");
    Serial.print(sensorValue);
    Serial.print(" , ");
    Serial.println(timeEnd-timeStart);
  }
}
