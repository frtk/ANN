/** 
 * file: ANN_APC_2.ino  
 * purpose: ANN APC control sketch
 * author: frtk@tetalab
 * notes: 
 *   - 2 digital pot 
 *   - 2 synthetizer pots (2 APC pots control) 
 *   - 2 input piezo sensor to control apc pots
 */

//#define DEBUG
 
// SPI library:
#include <SPI.h>
#include <stdint.h>
#include <math.h>


/** 
 * Serial console messages & debug
 */
boolean serial_on = true;
long serial_rate = 9600; 
boolean serial_dbug = false; //true;


/**
 * Digital pot - output sound control
 */
const int SYNTH_DIGIT_POT = 10; // slave select pin


/**
 *  Synthetizer pots
 */
const int n_pots = 2;
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
 * Piezo Sensors
 */
const int n_piezos = 2;
const int  piezo_pins[n_piezos] = { A0, A1 };
boolean piezo_trig[n_piezos] = { false, false };
int piezo_reading[n_piezos] = { 0 };

/**
 * Piezo handling
 */
int piezo_threshold = 1;
int piezo_read_max = 1023;
unsigned long piezo_read_delay = 50;
unsigned long piezo_read_last = 0; 

//unsigned long piezo1_timer


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
  //for (int i = 0; i < n_piezos; i++) {
  //    analogRead(piezo_pins[i];     
  //    piezo_reading[i] = analogRead(piezo_pins[i];
  //    if 
  //  }

  
  //
  if (change_count >= change_count_max) {
    //genChangeParams();
    genOppositePotsLevel();
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
  change_delay *= 5;
  change_count_max = random(1,3);
  change_count_max *= 100;
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
 *  func: genRandomPotsLevel()
 *  purpose: generate random synthetizer pots level
 */
void genOppositePotsLevel() {
  pots_rand_level[0] = random(256);
  pots_rand_level[1] = abs(255 - pots_rand_level[0]);
  //for (int i = 0; i < n_pots; i++) pots_rand_level[i] = random(256);
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




