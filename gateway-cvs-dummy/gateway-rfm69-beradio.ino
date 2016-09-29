/*

                  Hiveeyes gateway-rfm69-beradio

   Receives sensor data over radio and forward it to the UART interface,
   which is connected to the serial port of the gateway machine.

   Software release 0.5.1

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


int SENDERID2 = 2;
//Date/Time,Weight,Outside Temp,Outside Humid,Inside Temp,Inside Humid,Brood Temp,Voltage
String DATA2 = ",4.143,28.9,33.7,25.2,34.7,26.6,26.8,26.6,26.6,26.4,4.26";
int DATALEN2 = DATA2.length();

//Date/Time,Weight,Outside Temp,Outside Humid,Inside Temp,Inside Humid,H1 Temp,H2 Temp,H3 Temp,H4 Temp,H5 Temp,Voltage
int SENDERID3 = 3;
String DATA3 = ",1.001,16.5,83.9,16.2,81.6,16.1,3.9";
int DATALEN3 = DATA3.length();


void setup() {
  Serial.begin(115200);
}


void loop() {



/*
  if (radio.receiveDone())
  {
    Serial.println(radio.SENDERID, DEC)
    
    #theData.buff[i] = radio.DATA[i];
    Serial.print((char)radio.DATA[i]);
    Serial.println("\n")
    }

*/
/*
//#2
2016/05/09 15:07:22, 4.143, 28.9, 33.7, 25.2, 34.7, 26.6, 26.8, 26.6, 26.6, 26.4, 4.26
2016/05/09 15:27:22, 4.130, 26.1, 24.1, 25.2, 23.9, 27.1, 27.2, 27.2, 27.1, 27.0, 4.25
2016/05/09 15:47:22, 4.104, 28.2, 25.6, 28.1, 24.4, 25.7, 25.8, 25.6, 25.7, 25.6, 4.25 

//#3
2016/09/27 11:32:37,   1.001, 16.5, 83.9, 16.2, 81.6, 16.1, 3.94
2016/09/27 11:42:37,   1.007, 17.0, 82.4, 16.8, 80.1, 16.6, 3.94
2016/09/27 11:52:37,   1.006, 17.5, 79.2, 17.2, 76.6, 17.0, 3.94 
*/

    Serial.print(SENDERID2, DEC);
    Serial.println(DATA2);
    Serial.print(SENDERID3, DEC);
    Serial.println(DATA3);
    

}


