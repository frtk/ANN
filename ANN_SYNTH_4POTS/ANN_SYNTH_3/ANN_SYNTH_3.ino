/**
 * file: ANN_SYNTH_3.ino
 * purpose: ANN 4 pots synthetizer sketch
 * author: frtk@tetalab
 * notes:
 *   - 4 channel digital pot (output speakers master volume)
 *   - test ANNDigitalPot library
 */


//#define DEBUG

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
 *  - controls 4 synthetizer pots
 *  - arduino digital pin #10
 *  - 4 channels
 */
const int n_pots = 4;
unsigned char pots_level[n_pots] = { 0 };
boolean pots_rand_gen = true;
unsigned char pots_rand_level[n_pots] = { 0 };
int pots_max_val = 255;
ANNDigitalPot dpot(10, n_pots);
int pot_min = 125;
int pot_max = 180;



/**
 * Synthetizer sequences change
 */
typedef enum { CASC, NOISE, CASC2 } SEQType;
SEQType curr_seq;
int n_seq = 3;
unsigned long seq_change_delay = 10000;
unsigned long seq_change_last = 0;
boolean seq_change_on = false;
int seq_rand_nbr = 0;

unsigned long seq_delay = 100;
unsigned long seq_last = 0;


/**
 * Piezo Sensors
 */
const int n_piezos = 2;
const int  piezo_pins[n_piezos] = { A1, A2 };
boolean piezo_trig[n_piezos] = { false, false };
int piezo_reading[n_piezos] = { 0 };
int piezo_threshold = 1;
int piezo_read_max = 1023;
boolean piezo_active = false;
boolean piezo_seq_on = false;
unsigned long piezo_seq_delay = 1000;
unsigned long piezo_seq_last = 0;
unsigned long piezo_read_delay = 50;
unsigned long piezo_read_last = 0; 

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
  // change seed for random number generation 
  randomSeed(analogRead(A0));
  //
  dpot.init();  
  //
  initSynthSeq(); 

  //
  switchSEQType();

}


/**
 * func: loop()
 * purpose: synth seq change
 */
void loop() {

  // check piezo
  checkPiezoSensors();


 
  // handle synthetizer seq change
  handleSEQChange(); 
  
  // 
  updateSequence();

  // write synth pots level to digital pot
  //writePotsVolume();

  // reset trigger
  if (piezoTrigger()) resetTrigger();

}


/**
 * func: initSynthSeq()
 * purpose: initialize synthetizer sequence
 *  - set current effect        
 */
void initSynthSeq() {
  //
  curr_seq = CASC;

}


void checkPiezoSensors() {
  //if (piezo_active) {
    if (millis() - piezo_read_last >= piezo_read_delay) {
      for (int i = 0; i < n_piezos; i++) {
        analogRead(piezo_pins[i]);
        piezo_reading[i] = analogRead(piezo_pins[i]);
        if (piezo_reading[i] >= piezo_threshold) {
          piezo_trig[i] = true;
          //if (!piezo_seq_on) piezo_seq_on = true;
        }
        //else piezo_trig[i] = false;
      }
      piezo_read_last = millis();      
    }
  //}
}


boolean piezoTrigger() {
  for (int i = 0; i < n_piezos; i++) {
    if (piezo_trig[i]) return true;
  }
  return false;
}


boolean resetTrigger() {
  for (int i = 0; i < n_piezos; i++) piezo_trig[i] = false;

}



/**
 * func: handleSEQChange()
 * purpose: change synthetizer sequence
 */
void handleSEQChange() {
  if (millis() - seq_change_last >= seq_change_delay) {
    //
    switchSEQType();
    //
    seq_change_last = millis();
  }
}


/**
 * func: switchSEQType()
 * purpose: switch synthetizer sequence
 */ 
void switchSEQType() {
  seq_rand_nbr = random(1,100);
  seq_delay = seq_rand_nbr*30;
  seq_rand_nbr = random(1,20); // (1,15);
  seq_change_delay = seq_rand_nbr*1000; 

  if (seq_change_delay > 8000) {
    piezo_active = true;
    piezo_seq_delay = random(1,10);
    piezo_seq_delay *= 50;
  }
  else piezo_active = false;     


/*
  seq_rand_nbr = random(0,100);
  if (seq_rand_nbr >= 50) {
    if (curr_seq == CASC) curr_seq = NOISE;
  } else {
    if (curr_seq == NOISE) curr_seq = CASC; 
  }
  */
  //if (curr_seq == CASC) curr_seq = NOISE;
  //else curr_seq = CASC;

  /*
  seq_rand_nbr = random(n_seq);
  switch (seq_rand_nbr) {
    case 0:
      curr_seq = CASC;
      seq_delay = random(30,1500);
      //seq_delay = seq_rand_nbr*30;      break;
    case 1: 
      curr_seq = NOISE;
      seq_rand_nbr = random(1,10); // (1,15);
      seq_change_delay = seq_rand_nbr*100; 
      break;
    case 2:
      curr_seq = CASC2;
      break;         
  }
*/
  seq_rand_nbr = random(100);
  if (seq_rand_nbr < 70) {
    seq_delay = random(30,1500);
    seq_change_delay = random(1,20);
    seq_change_delay *= 1000;
    curr_seq = CASC;
  } else if (seq_rand_nbr >= 70 && seq_rand_nbr < 90) {
    seq_delay = random(30,1000);
    seq_change_delay = random(3,10);
    seq_change_delay *= 1000;
    curr_seq = CASC2;
  } else {
    seq_delay = random(30,1000);
    seq_change_delay = random(1,5);
    seq_change_delay *= 1000;
    curr_seq = NOISE; 
  }


  
}


/**
 * func: updateSequence()
 * purpose: 
 */
void updateSequence() {

  if (curr_seq == CASC) {
    if (millis() - seq_last >= seq_delay) {
      genRandomPotsLevel();
      writePotsVolume();
      seq_last = millis();
    }  
  } else if (curr_seq == NOISE) {
    if (millis() - seq_last >= seq_delay) {    
      //if (piezoTrigger() && random(100) > 70) genRandomPotsLevel();
      if (piezoTrigger()) genRandomPotsLevel();
      //reset();
      writePotsVolume();
      seq_last = millis();
    }    
  } else if (curr_seq == CASC2) {
    if (millis() - seq_last >= seq_delay) {
      //for (int i = 0;
      if (random(100) > 50) genRandomPotsLevel();
      writePotsVolume();
      seq_last = millis();
    }
  }

/*
  if (piezo_seq_on) {
    if (millis() - piezo_seq_last > piezo_seq_delay) {
        genRandomPotsLevel();
        writePotsVolume();
        piezo_seq_on = false;
    } else {
      if (piezoTrigger() && random(100) > 80) {
        genRandomPotsLevel();
        //writePotsVolume();
        //resetPots();
        //writePotsVolume(); 
      }     
    }
    piezo_seq_last = millis();
  } 
*/

/*
  seq_rand_nbr = random(2);
  switch (seq_rand_nbr) {
    case 0:
      if (millis() - seq_last >= seq_delay) {
        curr_seq = CASC;
        genRandomPotsLevel();
        //writePotsVolume();
        seq_last = millis();
      }
      break;
    case 1:
      curr_seq = NOISE;
      resetPots();
      //writePotsVolume();
      break;
    default:
      break;  
  }
  writePotsVolume();
  /*
  
  if (millis() - seq_last >= seq_delay) {
    if (curr_seq == CASC) {
        genRandomPotsLevel();
    } else if (curr_seq == NOISE) {
      resetPots();
    }
    seq_last = millis();
    //
    writePotsVolume();
  }
  //writePotsVolume();
  */
}

/**
 * func: writePotsVolume()
 * purpose: set digital pot channels to pots values
 */
void writePotsVolume() {
  for (int i = 0; i < n_pots; i++) {
    dpot.setChannelVal(i, pots_level[i]);
  }
  dpot.writeToDigitPot();
}



/**
 *  func: genRandomPotsLevel()
 *  purpose: generate random synthetizer pots level
 */
void genRandomPotsLevel() {
  for (int i = 0; i < n_pots; i++) pots_level[i] = random(256);
}

void resetPots() {
  pot_min = random(1, 180);
  pot_max = random(pot_min, 255);
  for (int i = 0; i < n_pots; i++) {
    if (random(100) >= 50) pots_level[i] = random(256); //random(pot_min, pot_max);
    
  }
}

#ifdef DEBUG
/**
 * func: printPotsVolume
 * purpose: print pots volume
 */  
void printPotsVolume() { 
  for (int i = 0; i < n_pots; i++) { 
    Serial.print(pots_level[i]);
    Serial.print(", ");
  }
  Serial.println("");
} 

void printSEQType() {
/*
  Serial.print("#### SPE change: new SPE = ");
  Serial.print(curr_spe);
  Serial.print(" ");   
  if (curr_spe == ROTATION) Serial.print(spe_rotation);
  else Serial.print(spe_config);
  Serial.print(" ");
  Serial.println(spe_change_delay);   
*/
}


#endif




