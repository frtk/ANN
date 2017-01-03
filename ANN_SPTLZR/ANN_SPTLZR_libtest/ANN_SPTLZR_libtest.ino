/**
 * file: ANN_SPTLZR_libtest.ino
 * purpose: ANN lib test
 * author: frtk@tetalab
 * notes:
 *   - 6 channel digital pot (output speakers master volume)
 *   - 2 spatialisation effect types (active speaker configs, sound rotations)
 *   - 2 piezo sensor (input to trigger spatialisation effects switch)
 */


//#define DEBUG

// SPI library
#include <SPI.h>
// C libs
#include <stdint.h>
#include <math.h>

#include "ANNDigitPot.h"

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
 *  Digital pot
 */
const int AUDIO_LEVEL_POT_PIN = 10; // slave select pin

ANNDigitPot<int, 6> bla;


/**
 *  6 Output Speakers
 */
const int n_speakers = 6;
int speakers_idx[n_speakers] = {0, 1, 2, 3, 4, 5 };
//boolean speakers_active[n_speakers] = { true, true, true, true, true, true };
unsigned char speakers_vol[n_speakers] = { 0 };
int speakers_max_vol = 255;




/**
 * Piezo Sensors
 */
const int n_piezos = 2;
const int  piezo_pins[n_piezos] = { A1, A2 };
boolean piezo_trig[n_piezos] = { false, false };
int piezo_reading[n_piezos] = { 0 };


/**
 * Piezo handling
 */
int piezo_threshold = 10;
int piezo_read_max = 1023;
//unsigned long piezo_read_delay = 50;
unsigned long piezo_read_last = 0;


/**
 *  Spatialization effects
 */
typedef enum { SPKRCONFIG, ROTATION } SPEffectType;
SPEffectType curr_spe;




/**
 * Spatialization effects : (active) speakers config
 */



/**
 * Spatialization effects : sound rotation
 */





/**
 * func: setup()
 * purpose: Arduino setup
 *  - start serial comm @serial_rate
 *  - initialize SPI
 *  - SPEffects vars init
 */
void setup() {
#ifdef DEBUG
  Serial.begin(serial_rate);
#endif
  //
  SPI.begin();
  // 
  initSPEffects();
  bla.setAllChannelToMax();
  bla.writeToDigitPot(AUDIO_LEVEL_POT_PIN);

}


/**
 * func: loop()
 * purpose: spatialisation effects handling main loop
 *  - piezo trigger handling
 *  - spatialisation effect sequence update
 */
void loop() {

 delay(5000);
 bla.mute();
 bla.writeToDigitPot(AUDIO_LEVEL_POT_PIN); 
 
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



