/*
  MandolynProtocol.h - Library for generating MandolynProtocol bitpattern for use wiht RF sender.
  Created by Robin Rene Risoy Jonsson, September 30, 2014.
  Released into the public domain.
*/
#ifndef MandolynProtocol_h
#define MandolynProtocol_h

#include "Arduino.h"

class MandolynProtocol
{
  public:
    MandolynProtocol(int pin);
	void sendRAW(const String pat);
	String intToBinString(unsigned int number, unsigned int numberOfBits);
	const void sendSensorSignal(const unsigned int ID,const unsigned int RH,const double Temp);
    void dot();
    void dash();
  private:
  	const int powOfTow(const unsigned int pow);
	const char intToChar(const int number);
	char getCheckBit(char pat[], int length);
	String getTempPattern(double temp);
	String getRHPattern(const unsigned int RH);
	String getChanaleCodePattern(const unsigned int chanalcode);
	String getHouseCodePattern(const unsigned int housecode);
	const String telldusID(const unsigned int ID);
	String intToBinStringV2(unsigned int number, unsigned int numberOfBits);
    unsigned int SIGNALPIN;
	unsigned int LEDPIN;
	unsigned int BITS;
	unsigned int ONEMS;
	double previosRH;
	String priviosRHBitpattern;
	double previosTemp;
	String priviosTempBitpattern;
	int previosSensorID;
	int previosSensorRH;
	double previosSensorTemp;
	String priviosSensorPattern;
	String c1;
	String c2;
};

#endif