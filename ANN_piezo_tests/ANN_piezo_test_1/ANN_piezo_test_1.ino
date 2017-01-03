/** 
 * file: ANN_piezo_test_1.ino  
 * purpose: piezo sensor testing as input   
 *          for ANN synthetizers control
 * doc: http://www.arduino.cc/en/Tutorial/Knock
 * author: frtk@tetalab
 */


//--- LED
const int ledPin = 13;      // led connected to digital pin 13
int ledState = LOW;         // variable used to store the last LED status, to toggle the light

//--- Piezo sensor
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 1;  // threshold value to decide when the detected sound is a knock or not
int sensorReading = 0;      // variable to store the value read from the sensor pin


/**
 * SETUP
 */
void setup() {
  pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
  Serial.begin(9600);       // use the serial port
}


/**
 * LOOP
 */
void loop() {
  // read the sensor and store it in the variable sensorReading:
  sensorReading = analogRead(knockSensor);
  Serial.print(sensorReading);   
  // if the sensor reading is greater than the threshold:
  if (sensorReading >= threshold) {
    // toggle the status of the ledPin:
    ledState = !ledState;
    // update the LED pin itself:
    digitalWrite(ledPin, ledState);
    Serial.print(" ");
    Serial.print("Knock!");
  }
  Serial.println(""); 
  
  delay(50);  // delay to avoid overloading the serial port buffer
}

