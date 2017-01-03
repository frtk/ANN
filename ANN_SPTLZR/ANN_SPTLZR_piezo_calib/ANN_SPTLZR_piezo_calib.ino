/** 
 * file: ANN_SPTLZR_piezo_calib.ino  
 * purpose: ANN spatializator piezo sensor calibration sketch
 * author: frtk@tetalab
 * notes: 
 *   - 2 input piezo sensor to trigger 
 *   - test piezo signal by muting speakers
 */

//#define DEBUG
 
// SPI library:
#include <SPI.h>
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
unsigned long tmp_time = 0;


// digital pot - output sound control
const int AUDIO_LEVEL_POT_PIN = 10; // slave select pin



/**
 *  Speakers
 */
const int n_speakers = 6;
int speakers_idx[n_speakers] = {0, 1, 2, 3, 4, 5 };
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


unsigned long p_switch_delay = 5000;
boolean p_switch_reset = false;
unsigned long p_switch_seq_last = 0;


/**
 * function: setup
 * purpose: Arduino setup
 *  - start serial comm @serial_rate
 *  - initialize SPI
 */
void setup() {
  // 
  Serial.begin(serial_rate);
  // 
  SPI.begin();
  //
  setAllSpeakersVolume(speakers_max_vol);
  //setAllSpeakersVolume(0);
  writeDigitPotInfo();

}


/**
 *
 * LOOP
 *
 */
void loop() {   

  // 
  if (millis() >= p_switch_delay) {
    p_switch_reset = true;
    //p_switch_seq_last = millis();
  }

/*
  // test piezo 0
  if (p_switch_reset) {
    // test piezo 0
    analogRead(piezo_pins[0]);
    piezo_reading[0] = analogRead(piezo_pins[0]);
    if (piezo_reading[0] >= piezo_threshold) {
      setAllSpeakersVolume(0);
      writeDigitPotInfo();
    }
  }
*/

  // test piezo 1
  if (p_switch_reset) {
    // test piezo 0
    analogRead(piezo_pins[1]);
    piezo_reading[1] = analogRead(piezo_pins[1]);
    if (piezo_reading[1] >= piezo_threshold) {
      setAllSpeakersVolume(0);
      writeDigitPotInfo();
    }
  }
  
}



/**
 *  
 *  GENERAL FUNCTIONS
 * 
 */

/*
 * func: setSpeakersVolume
 * purpose: set speakers volume
 */ 
void setSpeakersVolume() {
  for (int i = 0; i < n_speakers; i++) { 
    digitalPotWrite(i, speakers_vol[i]);
  }
#ifdef DEBUG  
  printSpeakersVolume();
#endif  
}

  
/**
 * func: muteAllSpeakers
 * purpose: mute all speakers
 */ 
void muteAllSpeakers() {
  for (int i = 0; i < n_speakers; i++) speakers_vol[i] = 0;  
}

/**
 * func: setAllSpeakersVolume
 * purpose: set all speakers volume to a value
 */ 
void setAllSpeakersVolume(int vol) {
  for (int i = 0; i < n_speakers; i++) speakers_vol[i] = vol;  
}

/*
 * func: setSpeakerVolume
 * purpose: set speaker volume i to a value
 */ 
void setSpeakerVolume(int idx, int vol) {
  speakers_vol[idx] = vol;  
}


/**
 *  POTS FUNCTIONS
 */
/**
 * func: writeDigitPotInfo()
 * purpose: send synth pots values
 *  to digital pot
 */
void writeDigitPotInfo() {
  for (int i = 0; i < n_speakers; i++) { 
    digitalPotWrite(i, speakers_vol[i]);
  }  
}
/**
 * func: digitalPotWrite(int address, int value)
 * purpose: write value to digital potentiometer
 *   - take the SS pin low to select the chip
 *   - send in the address and value via SPI
 *   - take the SS pin high to de-select the chip
 */ 
void digitalPotWrite(int address, int value) {
  digitalWrite(AUDIO_LEVEL_POT_PIN, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(AUDIO_LEVEL_POT_PIN, HIGH);
}



/**
 *  
 *  DEBUG FUNCTIONS
 * 
 */

#ifdef DEBUG
/**
 * func: printSpeakersVolume
 * purpose: print speakers volume
 */  
void printSpeakersVolume() { 
  for (int i = 0; i < n_speakers; i++) { 
    Serial.print(speakers_vol[i]);
    Serial.print(", ");
  }
  Serial.println("");
} 
#endif
