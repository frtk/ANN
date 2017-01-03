/** 
 * file: ANN_Rotator_3.ino  
 * purpose: ANN spatializator sketch
 * author: frtk@tetalab
 * notes: 
 *   - 1 input piezo sensor to trigger 
 *   - trigger sequence switch using piezo sensor
 *     (clockwise, anti-clockwise, random)
 *   - delay between sequence switch generated randomly
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
 *  Spatialization effects
 */
unsigned long speffects_seq_delay = 200;

unsigned long t_switch_seq = 10000;
int count = 0;


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
 * Input trigger from piezo sensors
 */
//
const int piezo1_pin = A2;
//
unsigned long piezo1_read_prev = 0;
unsigned long piezo1_read_delay = 50;
int piezo1_reading = 0;
int piezo1_threshold = 1;
boolean piezo1_state_on = true;
int piezo1_state = LOW;
int piezo1_state_prev = LOW;
boolean piezo1_trig = false;

unsigned long idle_timer_delay = 2000;
unsigned long idle_timer = 0;


/**
 * sequence switch 
 */
int trig_seq_idx = 0;
int trig_seq_idx_prev = 0;
unsigned long trig_seq_delay = 10000;
unsigned long trig_seq_timer = 0;



/**
 * function: setup
 * purpose: Arduino setup
 *  - start serial comm @serial_rate
 *  - initialize SPI
 *  - SPEffects vars init  
 */
void setup() {
  // 
  Serial.begin(serial_rate);
  // 
  SPI.begin();
  //
  randomSeed(analogRead(A0));
  //
  genSRMRandomSeq();
  setSRMRotDirSeq();     

  // setup for ANALOG NEURAL NETWORK
  srm_on = true;  

}


/**
 *
 * LOOP
 *
 */
void loop() {   

  // check piezo sensor 
  checkPiezoSensor();

  // Handle sequence switch
  seqSwitchHandling();

  // Sound Rotation algorithm  
  if (millis() - srm_t_last >= srm_rand_delay) {   
    // Update sequence 
    updateSequence();
    //
    setSpeakersVolume();
    //
    srm_t_last = millis();
  }  
  
}




/**
 *  
 *  GENERAL FUNCTIONS
 * 
 */

/**
 * func: checkPiezoSensor()
 * purpose: handle piezo sensor trigger detection
 */
void checkPiezoSensor() {
  if (millis() - piezo1_read_prev > piezo1_read_delay) {     
    // sensor reading (read pin twice and ignore first result)
    analogRead(piezo1_pin); // 
    piezo1_reading = analogRead(piezo1_pin);
    // compare with treshold
    if (piezo1_reading >= piezo1_threshold) setPiezoTrig(true); 
    else setPiezoTrig(false); 
#ifdef DEBUG
     //printPiezoInfo(piezo1_trig, piezo1_reading); //if (serial_debug) 
#endif
  }    
}


/**
 * func: setPiezoTrig()
 * purpose: set piezo trigger var
 */
void setPiezoTrig(boolean b) {
  piezo1_trig = b;
}

/**
 * func: piezoTrig()
 * purpose: is piezo sensor triggered ?
 */
boolean piezoTrig() {
  return piezo1_trig;
}



/**
 * func: seqSwitchHandling()
 * purpose: handle sequence switches when piezo is triggered
 */
void seqSwitchHandling() {
  if (piezoTrig()) {
    // check switch sequence timer
    if (millis() - trig_seq_timer >= trig_seq_delay) {
      // switch sequence randomly
      while (trig_seq_idx == trig_seq_idx_prev) trig_seq_idx = random(3); 
      // set new rotation sequence
      switchSequence(trig_seq_idx);
      // set timer
      trig_seq_timer = millis();
      // generate new switch sequence delay
      trig_seq_delay = 10000; //trig_seq_delay = random
      // dbug
#ifdef DEBUG
      printSewSwitchInfo(); 
#endif     
      //
      trig_seq_idx_prev = trig_seq_idx;
      // reset piezo trigger
      setPiezoTrig(false);
    }     
  }  
}




/**
 * func: switchSequence(int n)
 * purpose: switch rotation sequence
 */
void switchSequence(int n) {
  //
  switch (n) {
    case 0:
      setClockRot();
      break;
    case 1:
      setCounterClockRot();
      break;
    case 2:
      setRandomRot();
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


/*
 * func: mute()
 * purpose: mute all speakers
 */ 
void mute() {
  for (int i = 0; i < n_speakers; i++) { 
    digitalPotWrite(i, 0);
  }
}

/*
 * func: muteAllButOne()
 * purpose: mute all speakers except one
 */ 
void muteAllButOne(int spkr) {
  int idx = (spkr >= 0 && spkr < n_speakers) ? spkr : 0;
  for (int i = 0; i < n_speakers; i++) {
    if (i == idx) digitalPotWrite(i, speakers_vol[i]);  
    else digitalPotWrite(i, 0);
    //digitalPotWrite(i, 0);
  }
}
 
/*
 * func: updateSequence
 * purpose: update spatialization effect sequence
 */ 
void updateSequence() {
  muteAllSpeakers();	
  updateSRM();  
}

  
/*
 * func: muteAllSpeakers
 * purpose: mute all speakers
 */ 
void muteAllSpeakers() {
  for (int i = 0; i < n_speakers; i++) speakers_vol[i] = 0;  
}

/*
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
 *  
 *  SOUND ROTATION 
 * 
 */
 
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
 *  POTS FUNCTIONS
 */
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
/**
 * func: printSRMRandSeqParams()
 * purpose: print pot value and corresponding idx
 */ 
void printSRMRandSeqParams() {
  Serial.print("## SRM Seq:  NSTEPS= ");
  Serial.print(srm_rand_seq_nsteps);
  Serial.print("   |   DELAY=  ");
  Serial.print(srm_rand_delay);
  Serial.print("   |   CURR_IDX=  ");
  Serial.print(srm_curr_speaker_idx);
  Serial.print("   |  ROT_DIR =  ");
  if (srm_clockwise) Serial.println("CW");
  else Serial.println("CCW"); 
}
/**
 * func: printPiezoInfo()
 * purpose: print piezo sensor debug info
 */
void printPiezoInfo(boolean b, int val) {
  Serial.print(val);
  if (b) {
    Serial.print(" ");
    Serial.print("Knock!");
  }
  Serial.println();
}
/**
 * func: printSewSwitchInfo()
 * purpose: print sequence witch info
 */
void printSewSwitchInfo() {
  Serial.print("########################### SEQUENCE SWITCH:  FROM ");
  Serial.print(trig_seq_idx_prev);
  Serial.print(" TO ");
  Serial.println(trig_seq_idx);
 }
#endif




