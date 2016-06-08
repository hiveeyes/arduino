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
#include <WirelessHEX69.h>
#include <RFM69registers.h>
#include <avr/wdt.h>

/// Checks whether the last message received was a wireless programming request handshake
/// If so it will start the handshake protocol, receive the new HEX image and 
/// store it on the external flash chip, then reboot
/// Assumes radio has been initialized and has just received a message (is not in SLEEP mode, and you called CRCPass())
/// Assumes flash is an external SPI flash memory chip that has been initialized
void CheckForWirelessHEX(RFM69 radio, SPIFlash flash, boolean DEBUG, byte LEDpin)
{
  //special FLASH command, enter a FLASH image exchange sequence
  if (radio.DATALEN >= 4 && radio.DATA[0]=='F' && radio.DATA[1]=='L' && radio.DATA[2]=='X' && radio.DATA[3]=='?')
  {
    byte remoteID = radio.SENDERID;
    if (radio.DATALEN == 7 && radio.DATA[4]=='E' && radio.DATA[5]=='O' && radio.DATA[6]=='F')
    { //sender must have not received EOF ACK so just resend
      radio.send(remoteID, "FLX?OK",6);
    }
#ifdef SHIFTCHANNEL
    else if (HandleWirelessHEXDataWrapper(radio, remoteID, flash, DEBUG, LEDpin))
#else
    else if (HandleWirelessHEXData(radio, remoteID, flash, DEBUG, LEDpin))
#endif
    {
      if (DEBUG) Serial.print("FLASH IMG TRANSMISSION SUCCESS!\n");
      resetUsingWatchdog(DEBUG);
    }
    else
    {
      if (DEBUG) Serial.print("Timeout/Error, erasing written data ... ");
      //flash.blockErase32K(0); //clear any written data in first 32K block
      if (DEBUG) Serial.println("DONE");
    }
  }
}

#ifdef SHIFTCHANNEL
boolean HandleWirelessHEXDataWrapper(RFM69 radio, byte remoteID, SPIFlash flash, boolean DEBUG, byte LEDpin) {
  radio.sendACK("FLX?OK",6); //ACK the HANDSHAKE
  if (DEBUG) { Serial.println("FLX?OK (ACK sent)"); Serial.print("Shifting channel to "); Serial.println(radio.getFrequency() + SHIFTCHANNEL);}
  radio.setFrequency(radio.getFrequency() + SHIFTCHANNEL); //shift center freq by SHIFTCHANNEL amount
  boolean result = HandleWirelessHEXData(radio, remoteID, flash, DEBUG, LEDpin);
  if (DEBUG) { Serial.print("UNShifting channel to "); Serial.println(radio.getFrequency() - SHIFTCHANNEL);}
  radio.setFrequency(radio.getFrequency() - SHIFTCHANNEL); //restore center freq
  return result;
}
#endif

boolean HandleWirelessHEXData(RFM69 radio, byte remoteID, SPIFlash flash, boolean DEBUG, byte LEDpin) {
  uint32_t now=0;
  uint16_t tmp,seq=0;
  char buffer[16];
  uint16_t timeout = 3000; //3s for flash data
  uint16_t bytesFlashed=10;
#ifndef SHIFTCHANNEL
  radio.sendACK("FLX?OK",6); //ACK the HANDSHAKE
  if (DEBUG) Serial.println("FLX?OK (ACK sent)");
#endif

  //first clear the fist 32k block (dedicated to a new FLASH image)
  flash.blockErase32K(0);
  flash.writeBytes(0,"FLXIMG:", 7);
  flash.writeByte(9,':');
  now=millis();
    
  while(1)
  {
    if (radio.receiveDone() && radio.SENDERID == remoteID)
    {
      byte dataLen = radio.DATALEN;

      if (dataLen >= 4 && radio.DATA[0]=='F' && radio.DATA[1]=='L' && radio.DATA[2]=='X')
      {
        if (radio.DATA[3]==':' && dataLen >= 7) //FLX:_:_
        {
          byte index=3;
          tmp = 0;
          
          //read packet SEQ
          for (byte i = 4; i<8; i++) //up to 4 characters for seq number
          {
            if (radio.DATA[i] >=48 && radio.DATA[i]<=57)
              tmp = tmp*10+radio.DATA[i]-48;
            else if (radio.DATA[i]==':')
            {
              if (i==4)
                return false;
              else break;
            }
            index++;
          }

          if (DEBUG) {
            Serial.print("radio [");
            Serial.print(dataLen);
            Serial.print("] > ");
            PrintHex83((byte*)radio.DATA, dataLen);
          }

          if (radio.DATA[++index] != ':') return false;
          now = millis(); //got "good" packet
          index++;
          if (tmp==seq || tmp==seq-1) // if {temp==seq : new packet}, {temp==seq-1 : ACK was lost, host resending previously saved packet so must only resend the ACK}
          {
            if (tmp==seq)
            {
              seq++;
              for(byte i=index;i<dataLen;i++)
              {
                flash.writeByte(bytesFlashed++, radio.DATA[i]);
                if (bytesFlashed%32768==0) flash.blockErase32K(bytesFlashed);//erase subsequent 32K blocks (possible in case of atmega1284p)
              }
            }

            //send ACK
            tmp = sprintf(buffer, "FLX:%u:OK", tmp);
            if (DEBUG) Serial.println((char*)buffer);
            radio.sendACK(buffer, tmp);
          }
        }

        if (radio.DATA[3]=='?')
        {
          if (dataLen==4) //ACK for handshake was lost, resend
          {
            radio.sendACK("FLX?OK",6);
            if (DEBUG) Serial.println("FLX?OK resend");
          }
          if (dataLen==7 && radio.DATA[4]=='E' && radio.DATA[5]=='O' && radio.DATA[6]=='F') //Expected EOF
          {
#ifdef __AVR_ATmega1284P__
            if ((bytesFlashed-10)>65526) { //max 65536 - 10 bytes (signature)
              if (DEBUG) Serial.println("IMG exceeds 64k, refusing it");
              radio.sendACK("FLX?NOK",7);
              return false; //just return, let MAIN timeout
            }
#else //assuming atmega328p
            if ((bytesFlashed-10)>31744) {
              if (DEBUG) Serial.println("IMG exceeds 31k, refusing it");
              radio.sendACK("FLX?NOK",7);
              return false; //just return, let MAIN timeout
            }
#endif
            radio.sendACK("FLX?OK",6);
            if (DEBUG) Serial.println("FLX?OK");
            //save # of bytes written
            flash.writeByte(7,(bytesFlashed-10)>>8);
            flash.writeByte(8,(bytesFlashed-10));
            flash.writeByte(9,':');
            return true;
          }
        }
      }
      #ifdef LED //blink!
      pinMode(LEDpin,OUTPUT); digitalWrite(LEDpin,HIGH); delay(1); digitalWrite(LEDpin,LOW);
      #endif
    }
    
    //abort FLASH sequence if no valid packet received for a long time
    if (millis()-now > timeout)
    {
      return false;
    }
  }
}

// reads a line feed (\n) terminated line from the serial stream
// returns # of bytes read, up to 255
// timeout in ms, will timeout and return after so long
byte readSerialLine(char* input, char endOfLineChar, byte maxLength, uint16_t timeout)
{
  byte inputLen = 0;
  Serial.setTimeout(timeout);
  inputLen = Serial.readBytesUntil(endOfLineChar, input, maxLength);
  input[inputLen]=0;//null-terminate it
  Serial.setTimeout(0);
  //Serial.println();
  return inputLen;
}

/// returns TRUE if a HEX file transmission was detected and it was actually transmitted successfully
boolean CheckForSerialHEX(byte* input, byte inputLen, RFM69 radio, byte targetID, uint16_t TIMEOUT, uint16_t ACKTIMEOUT, boolean DEBUG)
{
  if (inputLen == 4 && input[0]=='F' && input[1]=='L' && input[2]=='X' && input[3]=='?') {
    if (HandleSerialHandshake(radio, targetID, false, TIMEOUT, ACKTIMEOUT, DEBUG))
    {
      Serial.println("\nFLX?OK"); //signal serial handshake back to host script
#ifdef SHIFTCHANNEL
      if (HandleSerialHEXDataWrapper(radio, targetID, TIMEOUT, ACKTIMEOUT, DEBUG))
#else
      if (HandleSerialHEXData(radio, targetID, TIMEOUT, ACKTIMEOUT, DEBUG))
#endif
      {
        Serial.println("FLX?OK"); //signal EOF serial handshake back to host script
        if (DEBUG) Serial.println("FLASH IMG TRANSMISSION SUCCESS");
        return true;
      }
      if (DEBUG) Serial.println("FLASH IMG TRANSMISSION FAIL");
      return false;
    }
  }
  return false;
}

boolean HandleSerialHandshake(RFM69 radio, byte targetID, boolean isEOF, uint16_t TIMEOUT, uint16_t ACKTIMEOUT, boolean DEBUG)
{
  long now = millis();

  while (millis()-now<TIMEOUT)
  {
    if (radio.sendWithRetry(targetID, isEOF ? "FLX?EOF" : "FLX?", isEOF?7:4, 2,ACKTIMEOUT))
    {
      if (radio.DATALEN == 6 && radio.DATA[0]=='F' && radio.DATA[1]=='L' && radio.DATA[2]=='X' && 
          radio.DATA[3]=='?' && radio.DATA[4]=='O' && radio.DATA[5]=='K')
        return true;
      if (radio.DATALEN == 7 && radio.DATA[0]=='F' && radio.DATA[1]=='L' && radio.DATA[2]=='X' && 
          radio.DATA[3]=='?' && radio.DATA[4]=='N' && radio.DATA[5]=='O' && radio.DATA[6]=='K')
        return true;
    }
  }

  if (DEBUG) Serial.println("Handshake fail");
  return false;
}

#ifdef SHIFTCHANNEL
boolean HandleSerialHEXDataWrapper(RFM69 radio, byte targetID, uint16_t TIMEOUT, uint16_t ACKTIMEOUT, boolean DEBUG) {
  radio.setFrequency(radio.getFrequency() + SHIFTCHANNEL); //shift center freq by SHIFTCHANNEL amount
  boolean result = HandleSerialHEXData(radio, targetID, TIMEOUT, ACKTIMEOUT, DEBUG);
  radio.setFrequency(radio.getFrequency() - SHIFTCHANNEL); //shift center freq by SHIFTCHANNEL amount
  return result;
}
#endif


boolean HandleSerialHEXData(RFM69 radio, byte targetID, uint16_t TIMEOUT, uint16_t ACKTIMEOUT, boolean DEBUG) {
  long now=millis();
  uint16_t seq=0, tmp=0, inputLen;
  byte remoteID = radio.SENDERID; //save the remoteID as soon as possible
  byte sendBuf[57];
  char input[115];
  //a FLASH record should not be more than 64 bytes: FLX:9999:10042000FF4FA591B4912FB7F894662321F48C91D6 

  while(1) {
    inputLen = readSerialLine(input);
    if (inputLen == 0) goto timeoutcheck;
    tmp = 0;
    
    if (inputLen >= 6) { //FLX:9:
      if (input[0]=='F' && input[1]=='L' && input[2]=='X')
      {
        if (input[3]==':')
        {
          byte index = 3;
          for (byte i = 4; i<8; i++) //up to 4 characters for seq number
          {
            if (input[i] >=48 && input[i]<=57)
              tmp = tmp*10+input[i]-48;
            else if (input[i]==':')
            {
              if (i==4)
                return false;
              else break;
            }
            index++;
          }
          //Serial.print("input[index] = ");Serial.print("[");Serial.print(index);Serial.print("]=");Serial.println(input[index]);
          if (input[++index] != ':') return false;
          now = millis(); //got good packet
          index++;
          byte hexDataLen = validateHEXData(input+index, inputLen-index);

          if (hexDataLen>0)
          {
            if (tmp==seq) //only read data when packet number is the next expected SEQ number
            {
              byte sendBufLen = prepareSendBuffer(input+index+8, sendBuf, hexDataLen, seq); //extract HEX data from input to BYTE data into sendBuf (go from 2 HEX bytes to 1 byte), +8 will jump over the header directly to the HEX raw data
              //Serial.print("PREP ");Serial.print(sendBufLen); Serial.print(" > "); PrintHex83(sendBuf, sendBufLen);
              
              //SEND RADIO DATA
              if (sendHEXPacket(radio, remoteID, sendBuf, sendBufLen, seq, TIMEOUT, ACKTIMEOUT, DEBUG))
              {
                sprintf((char*)sendBuf, "FLX:%u:OK",seq);
                Serial.println((char*)sendBuf); //response to host (python?)
                seq++;
              }
              else return false;
            }
          }
          else Serial.println("FLX:INV");
        }
        if (inputLen==7 && input[3]=='?' && input[4]=='E' && input[5]=='O' && input[6]=='F')
        {
          //SEND RADIO EOF
          return HandleSerialHandshake(radio, targetID, true, TIMEOUT, ACKTIMEOUT, DEBUG);
        }
      }
    }
    
    //abort FLASH sequence if no valid packet received for a long time
timeoutcheck:
    if (millis()-now > TIMEOUT)
    {
      Serial.print("Timeout receiving FLASH image from SERIAL, aborting...");
      //send abort msg or just let node timeout as well?
      return false;
    }
  }
  return true;
}

//returns length of HEX data bytes if everything is valid
//returns 0 if any validation failed
byte validateHEXData(void* data, byte length)
{
  //assuming 1 byte record length, 2 bytes address, 1 byte record type, N data bytes, 1 CRC byte
  char* input = (char*)data;
  if (length <12 || length%2!=0) return 0; //shortest possible intel data HEX record is 12 bytes
  //Serial.print("VAL > "); Serial.println((char*)input);

  uint8_t checksum=0;
  //check valid HEX data and CRC
  for (byte i=0; i<length;i++)
  {
    if (!((input[i] >=48 && input[i]<=57) || (input[i] >=65 && input[i]<=70))) //0-9,A-F
      return 0;
    if (i%2 && i<length-2) checksum+=BYTEfromHEX(input[i-1], input[i]);
  }
  checksum=(checksum^0xFF)+1;
  
  //TODO : CHECK for address continuity (intel HEX addresses are big endian)
  
  //Serial.print("final CRC:");Serial.println((byte)checksum, HEX);
  //Serial.print("CRC byte:");Serial.println(BYTEfromHEX(input[length-2], input[length-1]), HEX);

  //check CHECKSUM byte
  if (((byte)checksum) != BYTEfromHEX(input[length-2], input[length-1]))
    return 0;

  byte dataLength = BYTEfromHEX(input[0], input[1]); //length of actual HEX flash data (usually 16bytes)
  //calculate record length
  if (length != dataLength*2 + 10) //add headers and checksum bytes (a total of 10 combined)
    return 0;

  return dataLength; //all validation OK!
}

//returns the final size of the buf
byte prepareSendBuffer(char* hexdata, byte*buf, byte length, uint16_t seq)
{
  byte seqLen = sprintf(((char*)buf), "FLX:%u:", seq);
  for (byte i=0; i<length;i++)
    buf[seqLen+i] = BYTEfromHEX(hexdata[i*2], hexdata[i*2+1]);
  return seqLen+length;
}

//assume A and B are valid HEX chars [0-9A-F]
byte BYTEfromHEX(char MSB, char LSB)
{
  return (MSB>=65?MSB-55:MSB-48)*16 + (LSB>=65?LSB-55:LSB-48);
}

//return the SEQ of the ACK received, or -1 if invalid
boolean sendHEXPacket(RFM69 radio, byte targetID, byte* sendBuf, byte hexDataLen, uint16_t seq, uint16_t TIMEOUT, uint16_t ACKTIMEOUT, boolean DEBUG)
{
  long now = millis();
  
  while(1) {
    if (DEBUG) { Serial.print("RFTX > "); PrintHex83(sendBuf, hexDataLen); }
    if (radio.sendWithRetry(targetID, sendBuf, hexDataLen, 2, ACKTIMEOUT))
    {
      byte ackLen = radio.DATALEN;
      
      if (DEBUG) { Serial.print("RFACK > "); Serial.print(ackLen); Serial.print(" > "); PrintHex83((byte*)radio.DATA, ackLen); }
      
      if (ackLen >= 8 && radio.DATA[0]=='F' && radio.DATA[1]=='L' && radio.DATA[2]=='X' && 
          radio.DATA[3]==':' && radio.DATA[ackLen-3]==':' &&
          radio.DATA[ackLen-2]=='O' && radio.DATA[ackLen-1]=='K')
      {
        uint16_t tmp=0;
        sscanf((const char*)radio.DATA, "FLX:%u:OK", &tmp);
        return tmp == seq;
      }
    }

    if (millis()-now > TIMEOUT)
    {
      Serial.println("Timeout waiting for packet ACK, aborting FLASH operation ...");
      break; //abort FLASH sequence if no valid ACK was received for a long time
    }
  }
  return false;
}

void PrintHex83(uint8_t *data, uint8_t length) // prints 8-bit data in hex
{
  char tmp[length*2+1];
  byte first ;
  int j=0;
  for (uint8_t i=0; i<length; i++) 
  {
    first = (data[i] >> 4) | 48;
    if (first > 57) tmp[j] = first + (byte)39;
    else tmp[j] = first ;
    j++;

    first = (data[i] & 0x0F) | 48;
    if (first > 57) tmp[j] = first + (byte)39; 
    else tmp[j] = first;
    j++;
  }
  tmp[length*2] = 0;
  Serial.println(tmp);
}

/// Use watchdog to reset
void resetUsingWatchdog(boolean DEBUG)
{
  //wdt_disable();
  if (DEBUG) Serial.print("REBOOTING");
  wdt_enable(WDTO_15MS);
  while(1) if (DEBUG) Serial.print('.');
}