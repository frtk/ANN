/**
 * file: ANN_SPTLZR_2.ino
 * purpose: ANN spatializator node sketch
 * author: frtk@tetalab
 * notes:
 *   - 6 channel digital pot (output speakers master volume)
 *   - 2 spatialisation effect types (active speaker configs, sound rotations)
 *   - test ANNDigitalPot library
 */


#define DEBUG

// SPI library
#include <SPI.h>
// C libs
#include <stdint.h>
#include <math.h>
// ANN lib
#include "ANNDigitalPot.h"



/**
 *
 * GLOBAL VARIABLES
 *
 */

// Serial console messages & debug
boolean serial_on = true;
long serial_rate = 9600;
boolean serial_dbug = false; //true;


/**
 *  Digital pot: ANNDigitalPot(int pin, int nchan)   
 *  - controls 6 output speakers
 *  - arduino digital pin #10
 *  - 6 channels
 */
const int n_speakers = 6; 
ANNDigitalPot dpot(10, n_speakers);
int speakers_vol[n_speakers] = { 0 };


/**
 * Piezo Sensors & handling
 */
const int n_piezos = 2;
const int  piezo_pins[n_piezos] = { A1, A2 };
boolean piezo_trig[n_piezos] = { false, false };
int piezo_reading[n_piezos] = { 0 };
unsigned long piezo_read_delay[n_piezos] = { 1000, 1000 };
unsigned long piezo_read_last[n_piezos] = { 0, 0 };
int piezo_threshold = 10;
int piezo_read_max = 1023;


/**
 *  Spatialization effects (SPE) types
 */
typedef enum { SPKRCONFIG, ROTATION } SPEType;
SPEType curr_spe;
unsigned long spe_change_delay = 10000; // 20000;
unsigned long spe_change_last = 0;
boolean spe_change_on = false;
int spe_rand_nbr = 0;

/**
 * Spatialization effects : (active) speakers config
 */
typedef enum { ALL, SOME, MUTE } SPEConfigType;
SPEConfigType spe_config = ALL;


/**
 * Spatialization effects : sound rotation
 */
typedef enum { CWISE, CCWISE, RANDSEQ } SPERotType;
SPERotType spe_rotation = CWISE;




/**
 * func: setup()
 * purpose: Arduino setup
 *  - start serial comm @serial_rate
 *  - initialize ANN digital pot (& SPI)
 *  - SPEffects vars init
 */
void setup() {
  //
#ifdef DEBUG
  Serial.begin(serial_rate);
#endif
  //
  dpot.init();  
  //
  initSPEffects(); 
}


/**
 * func: loop()
 * purpose: spatialisation effects handling main loop
 *  - piezo trigger handling
 *  - spatialisation effect sequence update
 */
void loop() {
 
  // check piezo sensors state
  //checkPiezoSensors();

  // spatialisation effect handling


  // reset piezo sensors trigger detection
  //resetPiezoTrig();
  
  // write speakers volume to digital pot
  writeSpeakersVolume();

  delay(200);
}



/**
 * func: initSPEffects
 * purpose: initialize spatialisation effects variables
 *  - set current effect        
 */
void initSPEffects() {
  //
  curr_spe = SPKRCONFIG;
  //
}


/**
 * func: changeSPE()
 * purpose: change spatialisation effect 
 */
void changeSPE() {
  if (millis() - spe_change_last >= spe_change_delay) {
    //
    if (curr_spe == SPKRCONFIG) {
      curr_spe = ROTATION;
      // choose rotation sequence
      spe_rand_nbr = random(1,3);
      
    } else {
      curr_spe = SPKRCONFIG;
    }
 
    //
    spe_rand_nbr = random(1,100);
    if (spe_rand_nbr >= 70)    
    // generate new change params
    spe_change_delay = random(1,100);
    spe_change_delay *= 10000;
    if ()
    //
    spe_change_last = millis();
  }
}



/**
 * func: checkPiezoSensors()
 * purpose: handle piezo sensor trigger detection
 */
void checkPiezoSensors() {
  for (int i = 0; i < n_piezos; i++) {
    analogRead(piezo_pins[i]);
    piezo_reading[i] = analogRead(piezo_pins[i]);
    if (piezo_reading[i] >= piezo_threshold) piezo_trig[i] = true;    
  }
}


/**
 * func: piezoTrigger()
 * purpose: check if piezo sensors were triggered 
 */
boolean piezoTrigger() {
  for (int i = 0; i < n_piezos; i++) {
    if (piezo_trig[i]) return true;
  }
  return false;
}


/**
 * func: resetPiezoTrig()
 * purpose: reset piezo sensors trigger detection
 */ 
void resetPiezoTrig() {
  for (int i = 0; i < n_piezos; i++) {
    piezo_trig[i] = false;
  }
}
 
 

/**
 * func: writeSpeakersVolume()
 * purpose: set digital pot channels to speakers volume values
 */
void writeSpeakersVolume() {
  for (int i = 0; i < n_speakers; i++) {
    dpot.setChannelVal(i, speakers_vol[i]);
  }
  dpot.writeToDigitPot();
}


 




