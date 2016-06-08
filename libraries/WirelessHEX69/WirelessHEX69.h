// **********************************************************************************
// Library for facilitating wireless programming using an RFM69 transceiver (get library at: https://github.com/LowPowerLab/RFM69)
// and the SPI Flash memory library for arduino/moteino (get library at: http://github.com/LowPowerLab/SPIFlash)
// DEPENDS ON the two libraries mentioned above
// Install all three of these libraries in your Arduino/libraries folder ([Arduino > Preferences] for location of Arduino folder)
// **********************************************************************************
// Copyright Felix Rusu, LowPowerLab.com
// Library and code by Felix Rusu - felix@lowpowerlab.com
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// You should have received a copy of the GNU General    
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************
#ifndef _WirelessHEX69_H_
#define _WirelessHEX69_H_
#ifdef __AVR_ATmega1284P__
  #define LED           15 // Moteino MEGAs have LEDs on D15
#else
  #define LED           9 // Moteinos have LEDs on D9
#endif
#define SHIFTCHANNEL 1000000 //amount to shift frequency of HEX transmission to keep original channel free of the intense traffic

#ifndef DEFAULT_TIMEOUT
  #define DEFAULT_TIMEOUT 3000
#endif

#ifndef ACK_TIMEOUT
  #define ACK_TIMEOUT 20
#endif

#include <RFM69.h>
#include <SPIFlash.h>

//functions used in the REMOTE node
void CheckForWirelessHEX(RFM69 radio, SPIFlash flash, boolean DEBUG=false, byte LEDpin=LED);
void resetUsingWatchdog(boolean DEBUG=false);

boolean HandleWirelessHEXData(RFM69 radio, byte remoteID, SPIFlash flash, boolean DEBUG=false, byte LEDpin=LED);
#ifdef SHIFTCHANNEL
boolean HandleWirelessHEXDataWrapper(RFM69 radio, byte remoteID, SPIFlash flash, boolean DEBUG=false, byte LEDpin=LED);
#endif

//functions used in the MAIN node
boolean CheckForSerialHEX(byte* input, byte inputLen, RFM69 radio, byte targetID, uint16_t TIMEOUT=DEFAULT_TIMEOUT, uint16_t ACKTIMEOUT=ACK_TIMEOUT, boolean DEBUG=false);
boolean HandleSerialHandshake(RFM69 radio, byte targetID, boolean isEOF, uint16_t TIMEOUT=DEFAULT_TIMEOUT, uint16_t ACKTIMEOUT=ACK_TIMEOUT, boolean DEBUG=false);
boolean HandleSerialHEXData(RFM69 radio, byte targetID, uint16_t TIMEOUT=DEFAULT_TIMEOUT, uint16_t ACKTIMEOUT=ACK_TIMEOUT, boolean DEBUG=false);
#ifdef SHIFTCHANNEL
boolean HandleSerialHEXDataWrapper(RFM69 radio, byte targetID, uint16_t TIMEOUT=DEFAULT_TIMEOUT, uint16_t ACKTIMEOUT=ACK_TIMEOUT, boolean DEBUG=false);
#endif
boolean waitForAck(RFM69 radio, uint8_t fromNodeID, uint16_t ACKTIMEOUT=ACK_TIMEOUT);

byte validateHEXData(void* data, byte length);
byte prepareSendBuffer(char* hexdata, byte*buf, byte length, uint16_t seq);
boolean sendHEXPacket(RFM69 radio, byte remoteID, byte* sendBuf, byte hexDataLen, uint16_t seq, uint16_t TIMEOUT=DEFAULT_TIMEOUT, uint16_t ACKTIMEOUT=ACK_TIMEOUT, boolean DEBUG=false);
byte BYTEfromHEX(char MSB, char LSB);
byte readSerialLine(char* input, char endOfLineChar=10, byte maxLength=115, uint16_t timeout=1000);
void PrintHex83(byte* data, byte length);

#endif