/** 
 * file: ANN_SYNTH_6POTS_1.ino  
 * purpose: ANN 6 pots synthetizer control sketch
 * author: frtk@tetalab
 * notes: 
 *   - 6 digital pot
 *   - 6 synthetizer pots
 *   - 2 input piezo sensor to control synth pots effect
 */

//#define DEBUG
 
// SPI library:
#include <SPI.h>
#include <stdint.h>
#include <math.h>

/** 
 * GLOBAL VARIABLES
 */

// Serial console messages & debug
boolean serial_on = true;
long serial_rate = 9600; 
boolean serial_dbug = false; //true;

// digital pot - output sound control
const int SYNTH_DIGIT_POT = 10; // slave select pin

/**
 *  Synthetizer pots
 */
const int n_pots = 6;
unsigned char pots_level[n_pots] = { 0 };
boolean pots_rand_gen = true;
unsigned char pots_rand_level[n_pots] = { 0 };
int pots_max_val = 255;


// temporary switch synth pots values sequence
unsigned long change_delay = 2000;
unsigned long change_timer = 0;
int change_count_max = 3000;
int change_count = 0;

boolean pot_mvt_up = true;
int pot_mvt_idx = 0;
int pot_mvt_path = 1;


/**
 * function: setup
 * purpose: Arduino setup
 *  - start serial comm @serial_rate if DEBUG
 *  - initialize SPI
 *  - initialize synthetizer pots values 
 */
void setup() {
#ifdef DEBUG
  Serial.begin(serial_rate);
#endif 
  SPI.begin();
  initSynthPots();

 for (int i = 0; i < n_pots; i++) { 
    pots_level[i] = 125;
    digitalPotWrite(i, pots_level[i]);
    //digitalPotWrite(i, random(256));
 }  
  
}


/**
 *
 * LOOP
 *
 */
void loop() {   
  //

/*
  if (millis() - change_timer >= 30) {

    //if (random(100) > 80) writeDigitPotInfo(); 
    
    if (pot_mvt_up) { 
      pots_level[pot_mvt_idx] += pot_mvt_path;
      if (pots_level[pot_mvt_idx] > 255) {
        pot_mvt_up = false;
        pots_level[pot_mvt_idx] -= pot_mvt_path;  
      }
    } else {
      pots_level[pot_mvt_idx] -= pot_mvt_path;
      if (pots_level[pot_mvt_idx] < 0) {
        pot_mvt_up = true;
        pots_level[pot_mvt_idx] -= pot_mvt_path;
      }  
    }
    digitalPotWrite(pot_mvt_idx, pots_level[pot_mvt_idx]);
    change_timer = millis();

    
  
  }
 */
  /*
  change_count++;

  if (change_count >= change_count_max) {
    sequence1();
    change_count = 0;
    change_count_max = random(1,100);
    change_count_max *= 1000;
  }  
*/


  if (change_count >= change_count_max) {
    genChangeParams();
    change_count++;
  }  

  
  if (millis() - change_timer >= change_delay) {   
    setSynthPotsLevel();
    writeDigitPotInfo();
    change_timer = millis();
    //

    //genChangeParams();


#ifdef DEBUG
    printPotsLevel();
#endif
  }  



}


/**
 *  
 *  GENERAL FUNCTIONS
 * 
 */
/**
 * func: genChangeParams()
 * purpose: randomize new synth pots level change
 */
void genChangeParams() {
  change_delay = random(1, 10);
  change_delay *= 50;
  change_count_max = random(1,30);
  change_count_max *= 10;
  change_count = 0;  
}
 
 /**
 *  
 *  SYNTHETIZER POTS FUNCTIONS
 * 
 */
/**
 * func: sequence1()
 * purpose: have 5 pots close to same value and 1 random
 */
void sequence1() {
  int tmp_idx = random(n_pots);
  int med_pos = random(255);
  int tmp_val = med_pos;
  for (int i = 0; i < n_pots; i++) { 
    if (i == tmp_idx) {
      digitalPotWrite(i, random(255));
    } else {
      tmp_val = random(1,5);
      tmp_val = (random(100) > 50) ? med_pos + tmp_val : med_pos - tmp_val;  
      digitalPotWrite(i, tmp_val);
    }
  }    
}

/**
 * func: initSynthPots()
 * purpose: initialize synthetyzer pots values
 *   -  initialize the random number generator with a 
 *   random input (pin A0 unconnected)
 */
void initSynthPots() {
  randomSeed(analogRead(A0)); // init random number generator
  if (pots_rand_gen) genRandomPotsLevel(); // generate random pots level   
  genChangeParams(); // gen pots values change parameters
}
 
/**
 * func: setSynthPotsLevel(int p1, int p2, int p3, int p4)
 * 
 */
void setSynthPotsLevel() {
  for (int i = 0; i < n_pots; i++) { 
    if (pots_rand_gen) {
      genRandomPotsLevel();
      digitalPotWrite(i, pots_rand_level[i]);
    }
    else digitalPotWrite(i, pots_level[i]);
  }    
}
/**
 * func: setPotLevel(int pot, int val)
 * purpose: set synth pot "p" to "val"
 */ 
void setPotLevel(int pot, int val) {
  pots_level[pot] = val;  
}
/** 
 *  func: genRandomPotsLevel()
 *  purpose: generate random synthetizer pots level
 */
void genRandomPotsLevel() {
  for (int i = 0; i < n_pots; i++) pots_rand_level[i] = random(256);
}

/**
 *  DIGITAL POT FUNCTIONS
 */
/**
 * func: writeDigitPotInfo()
 * purpose: send synth pots values
 *  to digital pot
 */
void writeDigitPotInfo() {
  for (int i = 0; i < n_pots; i++) { 
    if (!pots_rand_gen) digitalPotWrite(i, pots_level[i]);
    else digitalPotWrite(i, pots_rand_level[i]);
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
  digitalWrite(SYNTH_DIGIT_POT, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(SYNTH_DIGIT_POT, HIGH);
}



/**
 *  
 *  DEBUG FUNCTIONS
 * 
 */

#ifdef DEBUG
/**
 * func: printPotsLevel()
 * purpose: print synth pots level
 */  
void printPotsLevel() { 
  for (int i = 0; i < n_pots; i++) { 
    if (!pots_rand_gen) Serial.print(pots_level[i]);
    else Serial.print(pots_rand_level[i]);
    Serial.print(", ");
  }
  Serial.println("");
} 
#endif




