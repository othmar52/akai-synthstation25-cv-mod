/*
  @see
  https://github.com/othmar52/akai-synthstation25-cv-mod

  thanks to:
  https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi-cv.pdf
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi_cv2.ino
  https://github.com/joeyoung/arduino_keypads/tree/master/Keypad_MC17
  https://github.com/mo-thunderz/lfo

*/

#define DACSIZE 4096
#define POT_FUZZY 6
#define GATE_RETRIGGER_DELAY_US 100 //time in microseconds to turn gate off in order to retrigger envelope

//MIDI variables
int8_t currentVibratoWave = 3; // sine as default
bool currentHold = false;
int8_t currentOctave;
int currentMidiNote; //the note currently being played


void setup() {
  Serial.begin(9600); //for debug, can't use midi at the same time!
  //Serial.begin(31250); // MIDI baudrate
  while( !Serial ){/*wait*/}   //for USB serial switching boards
  setupLedstrip();
  setupKeyboard();
  setupCvOut();
  setupVibrato();
  setupPitchBend();
  setupOctaveShift();
  setupHold();
  setupCvClockReset();
}

void loop() {
  loopKeyboardRead();
  loopVibratoRead();
  loopOctaveButtons();
  loopHoldButton();
  loopPitchBendPot();
  loopVibratoWaveButton();
  setNotePitch(currentMidiNote);
  loopLedstrip();
}
