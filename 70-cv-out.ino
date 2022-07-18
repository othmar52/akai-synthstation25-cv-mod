/*
                                  MCP4821
                                 top view
                              _______ _______
                             |       U       |
 to MCP4821 pin 6 (SHDN) <- -| 1 VDD  VOUT 8 |- -> 100 R -> PITCH CV OUTPUT JACK
   to Arduino Uno pin 10 <- -| 2 CS    VSS 7 |- -> GND
   to Arduino Uno pin 13 <- -| 3 SCK  SHDN 6 |- -> 5V (with 10 uF cap to GND)
   to Arduino Uno pin 11 <- -| 4 SDI  LDAC 5 |- -> GND
                             |_______________|

*/
 
/*

Arduino Uno SPI pins
10: CS/SS               <- we have to use this one for the MCP4821 CS/SS pin
11: MOSI (data output)  <- we have to use this one for the MCP4821 SDI pin
12: MISO (data input)
13: CLOCK               <- we have to use this one for the MCP4821 SCK pin

*/

// include the SPI library:
#include <SPI.h>

#define GATE_LED_PIN 12 //gate indicator
#define GATE_PIN 9 //gate control
#define SLAVE_SELECT_PIN 10 //spi chip select

#define DAC_BASE -3000 //-3V offset, TODO: we have to modify this value for tuning, right?
#define DAC_SCALE_PER_SEMITONE 83.333333333

int lastSendPitch = 0;

void setupCvOut() {
  pinMode (GATE_LED_PIN, OUTPUT); //gate indicator
  pinMode (GATE_PIN, OUTPUT); //gate CV
  //SPI stuff
  // set the slaveSelectPin as an output:
  pinMode (SLAVE_SELECT_PIN, OUTPUT);
  digitalWrite(SLAVE_SELECT_PIN,HIGH); //set chip select high
  // initialize SPI:
  SPI.begin(); 
  //dacWrite(1000); //set the pitch just for testing
  cvGateOff();
}

float getOctaveOffset() {
  return DAC_SCALE_PER_SEMITONE * currentOctave * 12;
}

float getPitchbendOffsetCV() {
  return pitchBendPercent * (DAC_SCALE_PER_SEMITONE*PITCH_BEND_SEMITONE_RANGE/100);
 
}
void setNotePitch(int note) {
  //receive a midi note number and set the DAC voltage accordingly for the pitch CV
  int dacValue = DAC_BASE+(note*DAC_SCALE_PER_SEMITONE)+ getPitchbendOffsetCV() + getOctaveOffset();


  if (vibratoPotAmpValue > 0) {
    lfo_class.setAmplOffset(dacValue);
    dacWrite(lfo_class.getWave(micros()));
    return;
  }
  dacWrite(dacValue);
}

void dacWrite(int value) {
  //write a 12 bit number to the MCP8421 DAC
  if (value < 0) {
    value = 0;
  }
  if (value > DACSIZE-1) {
    value = DACSIZE-1;
  }
  if (lastSendPitch == value) {
    // no need to write same value again
    return;
  }
  //Serial.print("new pitch value = ");
  //Serial.println(value);
  lastSendPitch = value;
  
  // take the SS pin low to select the chip:
  digitalWrite(SLAVE_SELECT_PIN,LOW);
  //send a value to the DAC
  SPI.transfer(0x10 | ((value >> 8) & 0x0F)); //bits 0..3 are bits 8..11 of 12 bit value, bits 4..7 are control data 
  SPI.transfer(value & 0xFF); //bits 0..7 of 12 bit value
  // take the SS pin high to de-select the chip:
  digitalWrite(SLAVE_SELECT_PIN,HIGH); 
}


void cvGateOff() {
  digitalWrite(GATE_PIN, LOW);
  digitalWrite(GATE_LED_PIN,LOW);
  setLedForGateClose();
}

void cvGateOn() {
  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(GATE_LED_PIN,HIGH);
  setLedForGateOpen();
}
