/*

                  Hiveeyes gateway-rfm69-csv-uart

   Receives CSV-based sensor data over radio and forwards it to the UART interface,
   which is connected to the serial port of the gateway machine.

   Copyright (C) 2014-2016  Clemens Gruber
   Copyright (C) 2014-2016  Richard Pobering <einsiedlerkrebs@ginnungagap.org>
   Copyright (C) 2014-2016  Andreas Motl <andreas.motl@elmyra.de>

   <https://hiveeyes.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see:
   <http://www.gnu.org/licenses/gpl-3.0.txt>,
   or write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

-------------------------------------------------------------------------

   Hiveeyes gateway sketch for Arduino based platforms

   This is a arduino sketch for the hiveeyes bee monitoring system.
   The purpose is to receive data strings in CSV format over a RFM69
   radio module and print them into serial out (RS-232), for further
   processing.

   Feel free to adapt this code to your own needs.

-------------------------------------------------------------------------

   Futher informations can be obtained at:

   hiveeyes                     https://hiveeyes.org/
   documentation                https://hiveeyes.org/docs/system/
   repository                   https://github.com/hiveeyes/
   beradio                      https://hiveeyes.org/docs/beradio/

-------------------------------------------------------------------------

*/


#include <RFM69.h>                      // https://github.com/LowPowerLab/RFM69
#include <SPI.h>

#define NODEID      1
#define NETWORKID   101
#define FREQUENCY   RF69_868MHZ         //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "demoKey4openHive"  //has to be same 16 characters/bytes on all nodes, not more not less!
#define SERIAL_BAUD 115200

#define SIMULATE


RFM69 radio;
bool promiscuousMode = false;           //set to 'true' to sniff all packets on the same network


void setup() {

    // Setup serial interface
    Serial.begin(SERIAL_BAUD);
    delay(10);

    #ifndef SIMULATE

    // Setup radio module
    radio.initialize(FREQUENCY, NODEID, NETWORKID);
    radio.encrypt(KEY);
    radio.promiscuous(promiscuousMode);

    #endif

}


#ifdef SIMULATE

// Weight,Outside Temp,Outside Humid,Inside Temp,Inside Humid,H1 Temp,H2 Temp,H3 Temp,H4 Temp,H5 Temp,Voltage
int SENDERID2 = 2;
String DATA2 = "4.143,28.9,33.7,25.2,34.7,26.6,26.8,26.6,26.6,26.4,4.26";

// Weight,Outside Temp,Outside Humid,Inside Temp,Inside Humid,Brood Temp,Voltage
int SENDERID3 = 3;
String DATA3 = "1.001,16.5,83.9,16.2,81.6,16.1,3.9";

#endif


void loop() {


#ifdef SIMULATE

    /*
    // Node #2
    2016/05/09 15:07:22, 4.143, 28.9, 33.7, 25.2, 34.7, 26.6, 26.8, 26.6, 26.6, 26.4, 4.26
    2016/05/09 15:27:22, 4.130, 26.1, 24.1, 25.2, 23.9, 27.1, 27.2, 27.2, 27.1, 27.0, 4.25
    2016/05/09 15:47:22, 4.104, 28.2, 25.6, 28.1, 24.4, 25.7, 25.8, 25.6, 25.7, 25.6, 4.25

    // Node #3
    2016/09/27 11:32:37,   1.001, 16.5, 83.9, 16.2, 81.6, 16.1, 3.94
    2016/09/27 11:42:37,   1.007, 17.0, 82.4, 16.8, 80.1, 16.6, 3.94
    2016/09/27 11:52:37,   1.006, 17.5, 79.2, 17.2, 76.6, 17.0, 3.94
    */

    Serial.print(SENDERID2, DEC);
    Serial.print(",,");
    Serial.println(DATA2);

    Serial.print(SENDERID3, DEC);
    Serial.print(",,");
    Serial.println(DATA3);

    delay(1000);

#else

    if (radio.receiveDone()) {

        // Prefix CSV payload with NODE ID of sender node.
        // Amendments for Make Faire 2016 to receive payloads from Open Hive hardware.
        Serial.print(radio.SENDERID, DEC);
        Serial.print(",");

        // Forward the payload
        for (byte i = 0; i < radio.DATALEN; i++) {
            Serial.print((char)radio.DATA[i]);
        }
        Serial.print("\n");

    }

#endif

}
