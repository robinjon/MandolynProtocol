/*
  MandolynProtocol.cpp - Library for generating MandolynProtocol bitpattern for use wiht RF sender.
  Created by Robin Rene Risoy Jonsson, September 30, 2014.
  Released into the public domain.
*/

#include "Arduino.h"
#include "MandolynProtocol.h"


MandolynProtocol::MandolynProtocol(int pin)
{
	SIGNALPIN = pin;
	pinMode(SIGNALPIN, OUTPUT);
	LEDPIN = 6; 
	BITS = 36;
	ONEMS = 900;  //uS Tweeking
	previosTemp = 0;
	priviosTempBitpattern = "";
	previosSensorID = 0;
	previosSensorRH = 0;
	previosRH = 0;
	priviosRHBitpattern = "";
	previosSensorTemp = 0;
	priviosSensorPattern = "";
	c1 = "1100";
	c2 = "110";
}

void MandolynProtocol::dot()
{
  digitalWrite(SIGNALPIN, HIGH);
  delay(250);
  digitalWrite(SIGNALPIN, LOW);
  delay(250);  
}

void MandolynProtocol::dash()
{
  digitalWrite(SIGNALPIN, HIGH);
  delay(1000);
  digitalWrite(SIGNALPIN, LOW);
  delay(250);
}

const int MandolynProtocol::powOfTow(const unsigned int pow){
  unsigned int result = 0;
  for(int i = 0; i < pow; i++){
    if(result == 0){
      result = 2;
    }else{
      result = result * 2;
    }
  }
  return result;
}

const char MandolynProtocol::intToChar(const int number){
  switch (number){
    case 1:
      return '1';
      break;
    case 0:
      return '0';
      break;
    default:
      return 'E';
      break;  
  }
}

char MandolynProtocol::getCheckBit(char pat[], int length){
  unsigned int result = 0;
  for(int i = 0; i < length; i++){ // counting number of ones in pattern
    if(pat[i] == '1'){result++;}
  }
  return intToChar(result%2);  // if odd return '1' else return '0'
}

void MandolynProtocol::sendRAW(const String pat){
  
  // init variables
  int cycle = 0;
  bool cyclesLeft = true;
  int state = 0;
  
  // setting start time
  int preCycleStartTime = micros();
  int cycleStartTime;
  
  // Bit string to char array
  char buffer[BITS+1];
  pat.toCharArray(buffer, BITS);
  
  // Addinf getting check bit in the end
  buffer[BITS-1] = getCheckBit(buffer, BITS);
  
  while(cyclesLeft){ // starting to send pattern
    cycleStartTime = micros();
    if((cycleStartTime - preCycleStartTime) >= ONEMS){
      preCycleStartTime = cycleStartTime;
      if(buffer[cycle/2] == '1'){
          state = !state; // switches state in the middle of the cycle
      }else{
        if((cycle+1)%2){
          state = !state; // switches state on the start of the cylce
        }
      }
      digitalWrite(SIGNALPIN, state);
      cycle++;
    }
    if(cycle >= ((BITS) * 2)+3){ // adds a '0' on the end of the pattern
      delayMicroseconds(ONEMS*2);
      cyclesLeft = false; // stops while loop
      digitalWrite(SIGNALPIN, LOW); // ends signal by setting pin to low
    }
  }
  delay(1);
  digitalWrite(SIGNALPIN, LOW);
  digitalWrite(LEDPIN, LOW);
}

String MandolynProtocol::intToBinString(unsigned int number, unsigned int numberOfBits){
  String binString = "";
  unsigned int _tempNumber = number;
  //Serial.println("test " + String(number));
  
  for(int i = numberOfBits-1; i >= 0; i--){
    if((_tempNumber - powOfTow(i)) >= 0 && _tempNumber != 0){
     binString += "1";
     _tempNumber -= powOfTow(i);
    }else{
    binString += "0";
    }
    
  }
  
  return binString;
}

String MandolynProtocol::getRHPattern(const unsigned int RH){
  if(previosRH == RH){return priviosRHBitpattern;}
  else{
  String pat = "";
  pat = intToBinStringV2(RH, 7);
  previosRH = RH;
  priviosRHBitpattern = pat;
  return pat;
  }
}

String MandolynProtocol::getTempPattern(double temp){
  /*if(previosTemp == temp){ return priviosTempBitpattern;}
  else{*/
  String result;
  int tempNumber = (temp * 128) + 6400;
  //Serial.println(String(tempNumber));
  result = intToBinStringV2(tempNumber, 15);////
  
  previosTemp = temp;
  priviosTempBitpattern = result;
  return result;
  //}
}

String MandolynProtocol::getChanaleCodePattern(const unsigned int chanalcode){
  String ch_pat;
  ch_pat = intToBinStringV2( chanalcode-1, 2);
  return ch_pat; 
}

String MandolynProtocol::getHouseCodePattern(const unsigned int housecode){
  String hc_pat;
  hc_pat = intToBinStringV2( housecode, 4);
  return hc_pat; 
}

const String MandolynProtocol::telldusID(const unsigned int ID){
  unsigned int houseCode = ID/10;
  unsigned int chanalCode = ID - (houseCode*10);
  if(chanalCode > 4 && chanalCode < 1){
  chanalCode = 1;
  // Unvalid chanal code
  }
  if(houseCode > 16 && houseCode < 0){
  houseCode = 0;
  // Unvalid Housecode code
  }
  String tmpResult = getHouseCodePattern(houseCode) + getChanaleCodePattern(chanalCode);
  //Serial.println(getChanaleCodePattern(chanalCode));
  return tmpResult;
}

const void MandolynProtocol::sendSensorSignal(const unsigned int ID,const unsigned int RH,const double Temp){
  String pat;
  if(previosSensorID == ID && previosSensorRH == RH & previosSensorTemp == Temp){pat = priviosSensorPattern;}
  else{
  pat = c1 + telldusID(ID) + c2 + getRHPattern(RH) + getTempPattern(Temp);
  previosSensorID = ID;
  previosSensorRH = RH;
  previosSensorTemp = Temp;
  priviosSensorPattern = pat;
  }
  //Serial.println("2: ");
  //Serial.println(pat);
  sendRAW(pat);
} 

String MandolynProtocol::intToBinStringV2(unsigned int number, unsigned int numberOfBits){
  String binString = "";
  int tmpBit;
  int bitCount = 0;
  while(number > 0){
    bitCount++;
    tmpBit = number % 2;
    number = number / 2;
    binString = String(tmpBit) + binString;
  }
  for(int i = 0; i < (numberOfBits - bitCount); i++){
    binString = "0"+ binString;
  }
  return binString;
}