/*

                  Hiveeyes gateway-rfm69-beradio

   Receives sensor data over radio and forward it to the UART interface,
   which is connected to the serial port of the gateway machine.

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
   The purpose is to receive data strings over a RFM69 radio module
   and print them into serial out (RS-232), for further processing.

   Since the data is are encapsulated in a BERadio character string
   the next hop needs to read, unwrap it and send it to a data sink.
   One option is BERadio pyhton module, which runs on arm's and send
   the data further via MQTT as a json payload.

   The creation of this code is strongly influenced by other projects, so
   credits goes to
   them: <https://hiveeyes.org/docs/beradio/README.html#credits>

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
#include <EmBencode.h>                  // https://github.com/jcw/embencode
#include <RFM69_ATC.h>                  // https://github.com/LowPowerLab/RFM69

#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_868MHZ         //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "www.hiveeyes.org"  //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         13
#define SERIAL_BAUD 115200
#define ACK_TIME    30                  // # of ms to wait for an ack
#define MAX_PAYLOAD_LENGTH 61
#define ENABLE_ATC                      //comment out this line to disable AUTO TRANSMISSION CONTROL
//#define RADIO_STATIC_POWER            //comment out for using ATC
//#define POWERLEVEL 5                  // for sending without an antenna
//#define DEBUG


#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif
bool promiscuousMode = false;           //set to 'true' to sniff all packets on the same network


typedef struct {
  int           nodeId;                 //store this nodeId
  char  buff[MAX_PAYLOAD_LENGTH];
  unsigned long uptime;                 //uptime in ms
  long         temp;
  int lenght;
} Payload;
Payload theData;

void setup() {
  Serial.begin(SERIAL_BAUD);
#ifdef DEBUG
  Serial.println("he-gateway");
#endif
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  //radio.setHighPower();               //uncomment only for RFM69HW!
#ifdef RADIO_STATIC_POWER
  radio.setPowerLevel(POWERLEVEL);      // lower power setting for missing antenna
#endif
  radio.encrypt(KEY);
  radio.spyMode(promiscuousMode);
  char buff[50];

#ifdef DEBUG
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
#endif
}

byte ackCount=0;

char embuf [61];
EmBdecode decoder (embuf, sizeof embuf);

void loop() {

  if (radio.receiveDone())
  {

/*
	   if (radio.ACKRequested())
	   {
		 byte theNodeID = radio.SENDERID;
		  radio.sendACK();
*/
#ifdef DEBUG
		 Serial.print(" - ACK sent.");
      }
 		/* When a node requests an ACK, respond to the ACK
		 and also send a packet requesting an ACK (every 3rd one only)
		 This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
		 */
#endif
#ifdef DEBUG
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    Serial.print(" [RX_RSSI:");Serial.print(radio.readRSSI());Serial.print("]");
#endif
/*
    if (promiscuousMode)
   {
#ifdef DEBUG
      Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
#endif
    }
*/

/*   if (radio.DATALEN != sizeof(Payload))
     Serial.print("Invalid payload received, not matching Payload struct!");
   else
    {
      theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else

*/

	   for (byte i = 0; i < radio.DATALEN; i++) {
		 theData.buff[i] = radio.DATA[i];
		 Serial.print((char)radio.DATA[i]);
   //      Serial.println(radio.DATA[i]);
	   }
#ifdef DEBUG
		Serial.print("\n nodeId=");
		Serial.print(theData.nodeId);
		 Serial.println("theData.buff= ");
		 Serial.print(theData.buff[0]);
		 Serial.print(theData.buff[1]);
		 Serial.print(theData.buff[2]);
		 Serial.print(theData.buff[10]);
		 Serial.print(theData.buff[11]);
#endif
		 Serial.print("\n");
#ifdef DEBUG
		 Serial.println("start Decoding");
		 data_decode();
		 Serial.println("\n decoding done");
#endif


/*
		 if (ackCount++%3==0)
		 {
#ifdef DEBUG
		   Serial.print(" Pinging node ");
		   Serial.print(theNodeID);
		   Serial.print(" - ACK...");
#endif
		   delay(3); //need this when sending right after reception .. ?
#ifdef DEBUG
		   if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
			 Serial.print("ok!");
		   else Serial.print("nothing");
#endif
		   }
	   }
*/
	   Serial.println();
   }
}

void data_decode() {
   Serial.print("foo");

  uint8_t bytes = 0;
   Serial.print(radio.DATALEN);
   for(byte i = 0; i < radio.DATALEN; i++) {


       bytes = decoder.process(theData.buff[i]);

	  if (bytes > 0) {
      Serial.print(" => ");
      Serial.print((int) bytes);
     Serial.println(" bytes");

      for (;;) {
        uint8_t token = decoder.nextToken();
        if (token == EmBdecode::T_END)
          break;
        switch (token) {
          case EmBdecode::T_STRING:
            Serial.print(" string: ");
            Serial.println(decoder.asString());
            break;
          case EmBdecode::T_NUMBER:
            Serial.print(" number: ");
            Serial.println(decoder.asNumber());
            break;
          case EmBdecode::T_DICT:
            Serial.println(" > dict");
            break;
          case EmBdecode::T_LIST:
            Serial.println(" > list");
            break;
          case EmBdecode::T_POP:
           Serial.println(" < pop");
            break;
        }
      }
      decoder.reset();
     }

  }
}
