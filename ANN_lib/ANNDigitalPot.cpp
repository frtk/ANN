#include "ANNDigitalPot.h"


/**
 *  func: ANNDigitalPot(int pin, int nchan)
 *  purpose: constructor method
 */
ANNDigitalPot::ANNDigitalPot(int pin, int nchan) {
  _pin = pin;
  _nchan = (nchan >= 0 && nchan < ANNDPOT_MAX_CHAN) ? nchan : 6;
}


/**
 *  func: init()
 *  purpose: initialisation
 */
void ANNDigitalPot::init() {
  //_nchan = (_nchan >= 0 && _nchan < ANNDPOT_MAX_CHAN) ? _nchan : 6;
  // init SPI
  SPI.begin();
}


/**
 *  func: reset()
 *  purpose: reset all channels level
 */
void ANNDigitalPot::reset() {
  for (int i = 0; i < _nchan; i++) {
    _channels[i] = 0;
  }
}


/**
 *  func: getChannelVal(int idx, int val)
 *  purpose: get channel "idx" value to "val"
 */
int ANNDigitalPot::getChannelVal(int idx, int val) {
  if (isValidIdx(idx) && isValidVal(val)) {
    return _channels[idx] = val;
  } else return -1;
}

/**
 *  func: setChannelVal(int idx, int val)
 *  purpose: set channel "idx" value to "val"
 */
void ANNDigitalPot::setChannelVal(int idx, int val) {
  if (isValidIdx(idx) && isValidVal(val)) {
    _channels[idx] = val;
  }
}


/**
 *  func: setChannelsVal(int val)
 *  purpose: set all channels value to "val"
 */
void ANNDigitalPot::setChannelsVal(int val) {
  for (int i = 0; i < _nchan; i++) {
    if (isValidVal(val)) _channels[i] = val;
  }
}


/**
 *  func: setChannelsToMax()
 *  purpose: set all channels value to MAX_VAL
 */
void ANNDigitalPot::setChannelsToMax() {
  for (int i = 0; i < _nchan; i++) {
    _channels[i] = ANNDPOT_MAX_VAL;
  }
}


/**
 *  func: isValidIdx(int idx)
 *  purpose: check if index "idx" is correct
 */
boolean ANNDigitalPot::isValidIdx(int idx) {
  return (idx >= 0 && idx < _nchan);
}


/**
 *  func: isValidVal(int val)
 *  purpose: check if value "val" is correct
 */
boolean ANNDigitalPot::isValidVal(int val) {
  return (val >= 0 && val < ANNDPOT_MAX_VAL);
}


/**
 *  func: writeToDigitPot()
 *  purpose: write channels value to digital pot
 */
void ANNDigitalPot::writeToDigitPot() {
  for (int i = 0; i < _nchan; i++) {
    digitalPotWrite(i, _channels[i]);
  }
}


/**
 *  func: digitalPotWrite(int address, int value)
 *  purpose: write channel value to digital pot
 */
void ANNDigitalPot::digitalPotWrite(int address, int value) {
  digitalWrite(_pin, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(_pin, HIGH);
}



