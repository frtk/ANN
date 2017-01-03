/**
 * file: ANN_SYNTH_6POTS_FLUTINE_1.ino
 * purpose: ANN 6 pots synthetizer sketch
 * author: frtk@tetalab
 * notes:
 *   - 6 channel digital pot (output speakers master volume)
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
const int n_pots = 6;
unsigned char pots_level[n_pots] = { 0 };
boolean pots_rand_gen = true;
unsigned char pots_rand_level[n_pots] = { 0 };
int pots_max_val = 255;
ANNDigitalPot dpot(10, n_pots);
int pot_min = 80;
int pot_max = 150;



/**
 * Synthetizer sequences change
 */
typedef enum { CASC, NOISE } SEQType;
SEQType curr_seq;
unsigned long seq_change_delay = 10000;
unsigned long seq_change_last = 0;
boolean seq_change_on = false;
int seq_rand_nbr = 0;

unsigned long seq_delay = 100;
unsigned long seq_last = 0;




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

}


/**
 * func: loop()
 * purpose: synth seq change
 */
void loop() {
 
  // handle synthetizer seq change
  handleSEQChange(); 
  
  // 
  updateSequence();

  // write synth pots level to digital pot
  //writePotsVolume();


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
  seq_rand_nbr = random(1,10);
  seq_delay = seq_rand_nbr*50;
  seq_rand_nbr = random(1,10);
  seq_change_delay = seq_rand_nbr*1000; 

/*
  seq_rand_nbr = random(0,100);
  if (seq_rand_nbr >= 50) {
    if (curr_seq == CASC) curr_seq = NOISE;
  } else {
    if (curr_seq == NOISE) curr_seq = CASC; 
  }
  */
  if (curr_seq == CASC) {
    curr_seq = NOISE;
    seq_rand_nbr = random(1,200);
    seq_change_delay = random(1,4000); //seq_rand_nbr*50;
  }
  else {
    curr_seq = CASC;
    seq_rand_nbr = random(1,5);
    seq_delay = seq_rand_nbr*500;  
  }
}


/**
 * func: updateSequence()
 * purpose: 
 */
void updateSequence() {

  if (curr_seq == CASC) {
    if (millis() - seq_last >= seq_delay) {
      if (random(100) >= 50) genRandomPotsLevel();
      writePotsVolume();
      seq_last = millis();
    }  
  } else if (curr_seq == NOISE) {
    resetPots();
    writePotsVolume();
  }

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
  for (int i = 0; i < n_pots; i++) {
    pots_level[i] = random(256); //random(20,40); //random(256);
    //if (random(100) >= 70) pots_level[i] = random(120,130);
    //else pots_level[i] = 125;
  }
}


void resetPots() {
  for (int i = 0; i < n_pots; i++) pots_level[i] = random(pot_min, pot_max);
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




