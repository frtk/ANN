#ifndef ANNDIGITALPOT_H
#define ANNDIGITALPOT_H

/**
 * file: ANNDigitPot.h
 * purpose: ANN digital pot handler class
 */
 
#include "Arduino.h"
#include <SPI.h>

#define ANNDPOT_MAX_VAL 255
#define ANNDPOT_MAX_CHAN 6

/**
 * Class Declaration 
 */
class ANNDigitalPot {

 private: 
  
  int _pin;     // arduino digital pin
  int _nchan;   // number of active channels (out of 6)
  int _channels[ANNDPOT_MAX_CHAN];


 public:
  
  ANNDigitalPot(int pin, int nchan);
 
  //
  void init();
  void reset();
  void setChannelVal(int idx, int val);
  void setChannelsVal(int val);
  void setChannelsToMax();
  int getChannelVal(int idx, int val);

  //
  boolean isValidIdx(int idx);
  boolean isValidVal(int val);
 
  // SPI handling
  void writeToDigitPot();
  void digitalPotWrite(int address, int val);


};

#endif
