/**
 * file: ANN_SPTLZR_3.ino
 * purpose: ANN spatializator node sketch
 * author: frtk@tetalab
 * notes:
 *   - 6 channel digital pot (output speakers master volume)
 *   - 2 spatialisation effect types (active speaker configs, sound rotations)
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
 *  - controls 6 output speakers
 *  - arduino digital pin #10
 *  - 6 channels
 */
const int n_speakers = 6; 
ANNDigitalPot dpot(10, n_speakers);
int speakers_vol[n_speakers] = { 0 };
int n_spkr_on = 6;
int speakers_max_vol = 255;



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
SPERotType spe_rotation = RANDSEQ;
/**
 *  Sound Rotation Module
 */
boolean srm_on = true;
int srm_nseq = 3;
boolean srm_clockwise = true;
int* srm_seq_ptr;
int srm_seq1[6] = {0, 1, 2, 3, 4, 5};
int srm_seq2[6] = {1, 0, 5, 4, 3, 2};
int srm_seq_nsteps = 6;
int srm_seq_curr_step = 0;
int srm_seq_nsubsteps = 8;
int srm_seq_curr_substep = 0;
int srm_prev_speaker_idx = 0;
int srm_curr_speaker_idx = 1;
int srm_next_speaker_idx = 2;
//
boolean srm_rand_on = true;
int srm_rand_seq_nsteps = 6;
int srm_rand_delay = 10;

unsigned long srm_t_last = 0;



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
  initSPEffects(); 

#ifdef DEBUG
  printSPEType();
#endif 

}


/**
 * func: loop()
 * purpose: spatialisation effects handling main loop
 *  - spatialisation effect sequence update
 */
void loop() {
 
  // spatialisation effect handling
  handleSPEChange(); 
  
  // 
  if (curr_spe == ROTATION) {
    if (millis() - srm_t_last >= srm_rand_delay) {   
      updateSequence();  
      writeSpeakersVolume();    
      srm_t_last = millis();
    }      
  }

  // write speakers volume to digital pot
  //writeSpeakersVolume();

#ifdef DEBUG
  //printSpeakersVolume();
#endif  
  
}



/**
 * func: initSPEffects
 * purpose: initialize spatialisation effects variables
 *  - set current effect        
 */
void initSPEffects() {
  //
  curr_spe = ROTATION;
  //
  genSRMRandomSeq();
  setSRMRotDirSeq();
}


/**
 * func: handleSPEChange()
 * purpose: change spatialisation effect 
 */
void handleSPEChange() {
  if (millis() - spe_change_last >= spe_change_delay) {
    //
    switchSPEType();
    //
    spe_change_last = millis();
  }
}


/**
 * func: switchSPEType()
 * purpose: switch spatialisation effect type 
 */
void switchSPEType() {
  //
  if (curr_spe == SPKRCONFIG) {
    curr_spe = ROTATION;
    switchRotationType();
    //spe_change_delay = random(1,10);
    //spe_change_delay *= 10000;
  } else {
    spe_rand_nbr = random(0, 100);
    //if (spe_rand_nbr >= 30) {
      curr_spe = SPKRCONFIG;
      switchSPKRConfigType();
      //spe_change_delay = random(1,10);
      //spe_change_delay *= 1000;   
    //} else {
    //  switchRotationType();
    //}
    
  }
#ifdef DEBUG
  printSPEType();
#endif  
}


/**
 * func: switchSPKRConfigType()
 * purpose: switch SPKR config spatialisation effect sequence
 */
void switchSPKRConfigType() {
  //
  spe_rand_nbr = random(3);
  //
  switch (spe_rand_nbr) {
    case 0:
      for (int i = 0; i < n_speakers; i++) speakers_vol[i] = speakers_max_vol;
      //dpot.setChannelsToMax(); // all active
      spe_change_delay = random(5,20);
      spe_change_delay *= 1000;   
      spe_config = ALL;
      break;
    case 1:
      for (int i = 0; i < n_speakers; i++) speakers_vol[i] = 0;
      //dpot.reset(); // mute all
      spe_config = MUTE;
      spe_change_delay = random(6,20);
      spe_change_delay *= 1000;       
      break;
    case 2:
      for (int i = 0; i < n_speakers; i++) speakers_vol[i] = 0;
      //dpot.reset(); // mute all
      spe_config = MUTE;
      spe_change_delay = random(4,15);
      spe_change_delay *= 1000;       
 /*
      speakers_vol[0] = speakers_max_vol;
      speakers_vol[4] = speakers_max_vol;
      speakers_vol[5] = speakers_max_vol;
      spe_change_delay = random(5,20);
      spe_change_delay *= 1000;       // need shuffle algo
      //dpot.; // some active
      spe_config = SOME;
*/
      break;
    default:
    break;
  }
  writeSpeakersVolume();
}


/**
 * func: switchRotationType()
 * purpose: switch rotation spatialisation effect sequence
 */
void switchRotationType() {
  //
  spe_rand_nbr = random(3);
  //
  switch (spe_rand_nbr) {
    case 0:
      setClockRot();
      spe_rotation = CWISE;
      spe_change_delay = random(5,30);
      spe_change_delay *= 1000; //1000;
      break;
    case 1:
      setCounterClockRot();
      spe_rotation = CCWISE;
      spe_change_delay = random(5,30);
      spe_change_delay *= 1000; //1000;
      break;
    case 2:
      setRandomRot();
      spe_rotation = RANDSEQ;
      spe_change_delay = random(30,60);
      spe_change_delay *= 1000; //1000;      
      break;
    default:
    break;
  }
}


/**
 * func: setClockRot()
 * purpose: set rotation sequence to "clockwise"
 * - set clockwise var
 * - set sequence ptr to clockwise array ptr
 * - stop random rotation
 */
void setClockRot() {
  srm_clockwise = true;
  srm_seq_ptr = (int*) srm_seq1;
  srm_rand_on = false;
  srm_rand_delay = 300;
}


/**
 * func: setCounterClockRot()
 * purpose: set rotation sequence to "counter clockwise"
 * - set clockwise var
 * - set sequence ptr to counter clockwise array ptr
 * - stop random rotation
 */
void setCounterClockRot() {
  srm_clockwise = false;
  srm_seq_ptr = (int*) srm_seq2;
  srm_rand_on = false;
  srm_rand_delay = 300;
}


/**
 * func: setRandomRot()
 * purpose: set rotation sequence to "random"
 * - start random rotation
 * - generate random rotation sequence
 */
void setRandomRot() {
  srm_rand_on = true;
  genNewRandSeq();
}


/*
 * function: nextItemIdx
 * purpose: find next item index
 */
int nextItemIdx(int idx, int nitems) {
  int next = 0;
  if (idx == nitems - 1) next = 0;
  else next = idx + 1;
  return next;
}

/*
 * func: prevItemIdx
 * purpose: find prev item index
 */
int prevItemIdx(int idx, int nitems) {
  int prev = 0;
  if (idx == 0) prev = nitems - 1;
  else prev = idx - 1;
  return prev;
}

/*
 * func: muteAllSpeakers
 * purpose: mute all speakers
 */
void muteAllSpeakers() {
  for (int i = 0; i < n_speakers; i++) speakers_vol[i] = 0;
}


/*
 * func: updateSequence
 * purpose: update spatialization effect sequence
 */
void updateSequence() {
  muteAllSpeakers();
  updateSRM();
}

/**
 * func: updateSRM
 * purpose: update SRM spatialization effect sequence
 *  - seq 1: clockwise rotation
 *  - seq 2: counter clockwise rotation
 *  - seq 3: random jump and direction change
 */ 
void updateSRM() {
  // update seq step and set current speaker idx
  int nseqsteps = srm_rand_on ? srm_rand_seq_nsteps : srm_seq_nsteps;
  boolean reset_seq = false;
  if (srm_clockwise) {
    if (srm_seq_curr_substep < srm_seq_nsubsteps - 1) srm_seq_curr_substep++;   
    else {
      srm_seq_curr_substep = 0;
      if (srm_seq_curr_step < nseqsteps - 1) {
        srm_seq_curr_step++;
      } else {
        srm_seq_curr_step = 0;
        reset_seq = true;
      }
    }
  } else { 
    if (srm_seq_curr_substep > 0) srm_seq_curr_substep--;   
    else {
      srm_seq_curr_substep = srm_seq_nsubsteps - 1;
      if (srm_seq_curr_step < nseqsteps - 1) {
        srm_seq_curr_step++;
      } else {
        srm_seq_curr_step = 0;
        reset_seq = true;
      }
    }   
  }

  // set prev and next speakers idx
  srm_curr_speaker_idx = getSRMCurrSpeakerIdx();      
  if (reset_seq && srm_rand_on) {
    genSRMRandomSeq();
    setSRMRotDirSeq();
  }
  srm_prev_speaker_idx = getSRMPrevSpeakerIdx();
  srm_next_speaker_idx = getSRMNextSpeakerIdx();

  // calculate new volumes values
  muteAllSpeakers();
  speakers_vol[srm_prev_speaker_idx] = speakers_max_vol*(1 - sin(srm_seq_curr_substep*M_PI/16));
  speakers_vol[srm_curr_speaker_idx] = speakers_max_vol;
  speakers_vol[srm_next_speaker_idx] = speakers_max_vol*sin(srm_seq_curr_substep*M_PI/16);          
} 


/**
 * func: genNewRandSeq()
 * purpose: generate new random sequence
 */
void genNewRandSeq() {
  genSRMRandomSeq();
  setSRMRotDirSeq();  
}


/*
 * func: genSRMRandomSeq
 * purpose: generate SRM random sequence
 */ 
void genSRMRandomSeq() {
  srm_rand_seq_nsteps = random(4,12);
  srm_rand_delay = random(20, 200);
  srm_curr_speaker_idx = random(6);
  int rand = random(100);
  srm_clockwise = rand < 50 ? true : false;
}

/*
 * func: switchRotDir()
 * purpose: switch rotation direction
 */ 
void switchRotDir() {
  if (srm_clockwise) srm_clockwise = false;
  else srm_clockwise = true;
  setSRMRotDirSeq();
}


/*
 * func: setSRMRotDirSeq
 * purpose: set SRM sequence rotation dir
 */ 
void setSRMRotDirSeq() {
  if (srm_clockwise) srm_seq_ptr = (int*) srm_seq1;
  else srm_seq_ptr = (int*) srm_seq2;
}

/*
 * func: getSRMCurrSpeakerIdx()
 * purpose: get SRM effect current speaker index
 */ 
int getSRMCurrSpeakerIdx() {
  //
  return srm_seq_ptr[srm_seq_curr_step%n_speakers];
}

/*
 * func: getSRMNextSpeakerIdx()
 * purpose: get SRM effect next speaker index
 */ 
int getSRMNextSpeakerIdx() {
  int idx = 0;	
  if (srm_curr_speaker_idx == n_speakers - 1) idx = 0;
  else idx = srm_curr_speaker_idx + 1;	
  return idx;
}

/*
 * func: getSRMPrevSpeakerIdx()
 * purpose: get SRM effect prev speaker index
 */ 
int getSRMPrevSpeakerIdx() {
  int idx = 0;	
  if (srm_curr_speaker_idx == 0) idx = n_speakers - 1;
  else idx = srm_curr_speaker_idx - 1;	
  return idx;
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

void printSPEType() {
  Serial.print("#### SPE change: new SPE = ");
  Serial.print(curr_spe);
  Serial.print(" ");   
  if (curr_spe == ROTATION) Serial.print(spe_rotation);
  else Serial.print(spe_config);
  Serial.print(" ");
  Serial.println(spe_change_delay);   
}


#endif




