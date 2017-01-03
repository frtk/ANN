/** 
 * file: ANN_piezo_test_3.ino  
 * purpose: piezo sensor testing as input   
 *          for ANN synthetizers control
 * doc: http://www.arduino.cc/en/Tutorial/Knock
 * author: frtk@tetalab
 */

//#define DEBUG

unsigned loop_delay_max = 50;
unsigned long loop_delay = 0;

//--- LED
const int led_pin = 13;      // led connected to digital pin 13
unsigned long led_delay_max = 50;
unsigned long led_delay_min = 10;
unsigned long led_delay = 0;
 
//--- Piezo sensor
unsigned 
const int piezo_pin = A0; // the piezo is connected to analog pin 0
const int piezo_threshold = 1;  // threshold value to decide when the detected sound is a knock or not
int piezo_reading = 0;      // variable to store the value read from the sensor pin
int piezo_reading_max = 60;
boolean piezo_trig = false;

unsigned long piezo_val_max = 0;

/**
 * SETUP
 */
void setup() {

#ifdef DEBUG
  Serial.begin(9600); 
#endif

  // Led pin
  pinMode(led_pin, OUTPUT); 

  // turn led off
  turnLedOff();
}


/**
 * LOOP
 */
void loop() {
 
  // piezo reading
  piezoHandling();
  
  // led handling
  ledHandling();
  
  //delay to avoid overloading the serial port buffer
  //loop_delay = piezo_trig ? loop_delay_max - led_delay : loop_delay_max;
  //delay(loop_delay); 
}


/**
 * func: piezoHandling()
 * purpose: piezo sensor reading
 */
void piezoHandling() {
  //
  if (millis() - piezo_last > piezo_read_delay) {     
    // sensor reading
    analogRead(piezo_pin); // read pin twice and ignore first result
    piezo_reading = analogRead(piezo_pin);
    // compare with treshold
    if (piezo_reading >= piezo_threshold) {
      piezo_trig = true;
      // compute led delay
      if (piezo_reading > piezo_reading_max) piezo_reading_max = piezo_reading;
      led_delay = map(piezo_reading, piezo_threshold, piezo_reading_max, led_delay_min, led_delay_max);
      led_on = true;
      // turn on/off led 
      turnLedOn();
      delay(led_delay);
      turnLedOff(); 
    } else {
      piezo_trig = false;
      led_on = false; 
    }

#ifdef DEBUG
    printPiezoInfo(piezo_trig, piezo_reading);
#endif

  }
  
}


/**
 * func: ledHandling()
 * purpose handle led behaviour
 */
void ledHandling() {
  //
  if (led_on) {
    if (millis - led_last > led_delay) {
      
    }
  }
}


/**
 * func: turnLedOn
 * purpose: turn led on
 */
void turnLedOn() {
  digitalWrite(led_pin, HIGH);
}

/**
 * func: turnLedOff
 * purpose: turn led off
 */
void turnLedOff() {
  digitalWrite(led_pin, LOW);
}


/**
 * func: print piezo sensor info
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



