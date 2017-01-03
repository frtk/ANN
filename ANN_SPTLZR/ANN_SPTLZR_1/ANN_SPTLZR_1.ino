/**
 * file: ANN_SPTLZR_1.ino
 * purpose: ANN spatializator node sketch
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
}


/**
 * func: loop()
 * purpose: spatialisation effects handling main loop
 *  - piezo trigger handling
 *  - spatialisation effect sequence update
 */
void loop() {
 
  // piezo handling

  // spatialisation effect handling

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
 * func: muteSpeakers()
 * purpose: mute all speakers
 */
void muteSpeakers() {
  for (int i = 0; i < n_speakers; i++) speakers_vol[i] = 0;
}


/**
 * func: maxSpeakersLevel()
 * purpose: set speakers volume to maximum value
 */
void maxSpeakersLevel(int val) {
  for (int i = 0; i < n_speakers; i++) speakers_vol[i] = speakers_max_vol;
}


/**
 * func: setSpeakersLevel(int val)
 * purpose: set speakers volume to "val"
 */
void setSpeakersLevel(int val) {
  for (int i = 0; i < n_speakers; i++) speakers_vol[i] = val;
}


/**
 * func: setSpeakerLevel(int idx, int val)
 * purpose: set speaker "idx" to volume "val"
 */
void setSpeakerLevel(int idx, int val) {
  if (isValidSpeakerIdx(idx)) speakers_vol[idx] = val;
}


/**
 * func: isValidSpeakerIdx(int idx)
 * purpose: check if speaker idx is valid
 */
void isValidSpeakerIdx(int idx) {
  return (idx >= 0 && idx < n_speakers);
}






