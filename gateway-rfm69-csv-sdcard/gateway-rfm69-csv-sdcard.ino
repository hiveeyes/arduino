/*
  Open Hive | Gateway Yun2SD

  Copyright (C) 2016 by Clemens Gruber


  Changes
  -------
  2015-10 Clemens Gruber | initial sketch


  GNU GPL v3 License
  ------------------
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  Licence can be viewed at
  http://www.gnu.org/licenses/gpl-3.0.txt

*/

// Prepare your SD card creating an empty folder in the SD root
// named "arduino". This will ensure that the Yun will create a link
// to the SD to the "/mnt/sd" path.
//
// You can remove the SD card while the Linux and the
// sketch are running but be careful not to remove it while
// the system is writing to it.

// libraries
// SD, Yun
#include <FileIO.h>

// radio
#include <RFM69.h>  // RFM69 itself
#include <SPI.h>    // SPI (for RTC)
#define NODEID     1    // must be unique for each node on the same network
#define NETWORKID  101  // the same on all nodes that talk to each other
#define FREQUENCY  RF69_868MHZ  // Match frequency to the hardware version of the radio
#define ENCRYPTKEY "demoKey4openHive"  // exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW  // uncomment only for RFM69_H_W! Leave out if you have RFM69W!
#define ACK_TIME   30 // max # of ms to wait for an ack

#define LED        13 // Moteino MEGAs have LEDs on D15

RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network


void setup() {
  // Initialize Yun Bridge and FileSystem
  Bridge.begin();
  FileSystem.begin();

  //debug
  Serial.begin(9600);

  // radio
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  #ifdef IS_RFM69HW  // only for RFM69_H_W!
    radio.setHighPower();
  #endif
  radio.encrypt(ENCRYPTKEY);
  radio.spyMode(promiscuousMode);

  Serial.println(FREQUENCY);
}


void loop() {
  if (radio.receiveDone()) {
    Serial.print("[Node");Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    if (promiscuousMode) {
      Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
    }

    char dataset[radio.DATALEN+1];
    for (byte i = 0; i < radio.DATALEN; i++) {
      dataset[i] = (char)radio.DATA[i];
    }
    dataset[radio.DATALEN] = '\0'; // add closing \0

    // extract date from timestamp
    char dateChar[11];
    // starting by position 0 extract 10 characters
    memcpy(dateChar, &dataset[0], 10 );
    dateChar[10] = '\0';  // and add closing \0

    // replace "/" with "-" for propper file´names
    char *pch;
    do {
      pch = strstr (dateChar,"/");
      strncpy (pch,"-",1);
      puts (dateChar);
    } while (pch != NULL);

    char path[40];
    char pathPrefix[] = "/mnt/sd/arduino/www/";
    char folderPrefix[] = "node";
    char fileExt[] = ".txt";
    sprintf(path,"%s%s%03i",pathPrefix,folderPrefix,radio.SENDERID);
    FileSystem.mkdir(path);

    char pathAndFile[60];
    sprintf(pathAndFile,"%s/%s%s",path,dateChar,fileExt);
    Serial.print(pathAndFile);

    File dataFile = FileSystem.open(pathAndFile, FILE_APPEND);

    // if the file is available, write to it:
    if (dataFile) {
      dataFile.print(dataset);
      dataFile.println();
      dataFile.close();
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening file");
    }

    Serial.print(" [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");



    Serial.println();
    Blink(LED,300);
  }
}


void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
