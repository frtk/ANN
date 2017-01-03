/** 
 * file: ANN_SYNTH_2.ino  
 * purpose: ANN synthetiwer control sketch
 * author: frtk@tetalab
 * notes: 
 *   - 6 digital pot
 *   - 4 synthetizer pots
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
const int n_pots = 4;
unsigned char pots_level[n_pots] = { 0 };
boolean pots_rand_gen = true;
unsigned char pots_rand_level[n_pots] = { 0 };
int pots_max_val = 255;


// temporary switch synth pots values sequence
unsigned long change_delay = 2000;
unsigned long change_timer = 0;
int change_count_max = 10;
int change_count = 0;

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
}


/**
 *
 * LOOP
 *
 */
void loop() {   
  //
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




