/*
  @see
  https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi-cv.pdf
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi_cv2.ino
  https://github.com/joeyoung/arduino_keypads/tree/master/Keypad_MC17
  https://github.com/mo-thunderz/lfo

*/

#include <Keypad.h>
#include "libs/keypad-MCP23017/Keypad_MC17.h"
#include "libs/keypad-MCP23017/Keypad_MC17.cpp"
#include <MIDI.h>

#include "libs/lfo/lfo.h"
#include "libs/lfo/lfo.cpp"

// include the SPI library:
#include <SPI.h>

#define I2CADDR 0x20        // address of MCP23017 chip on I2C bus

#define DACSIZE 4096

#define VIBRATO_LOOP_WAVES_BUTTON_PIN 5 // Arduino Uno pin the loop waveform momentary switch is connected to
#define VIBRATO_AMP_PIN A2              // Arduino Uno pin the amplitude potentiometer is connected to
#define VIBRATO_FREQ_PIN A1             // Arduino Uno pin the frequency potentiometer is connected to
#define VIBRATO_HZ_LOWER 0.2
#define VIBRATO_HZ_UPPER 13
#define VIBRATO_AMP_LOWER 0
#define VIBRATO_AMP_UPPER 20

// we do not use the full range of the pot as we use the original "mod wheel" unit of AKAI synthstation 25
#define VIBRATO_FREQ_POT_LOWER 367
#define VIBRATO_FREQ_POT_UPPER 670



#define KBD_R1 0
#define KBD_R2 1
#define KBD_R3 2
#define KBD_R4 3
#define KBD_C1 4
#define KBD_C2 5
#define KBD_C3 6
#define KBD_C4 7
#define KBD_C5 8
#define KBD_C6 9
#define KBD_C7 10


#define OCTAVE_DOWN_BUTTON_PIN 2
#define OCTAVE_UP_BUTTON_PIN 3
#define OCTAVE_SHIFT_RANGE 2 // 2 octaves up, 2 octaves down

#define HOLD_BUTTON_PIN 4


#define OCTAVE_PIN A3
#define PITCH_BEND_PIN A0 // pitch bend potentiometer
// we do not use the full range of the pot as we use the original "pitch bend" unit of AKAI synthstation 25
#define PITCH_BEND_LOWER 360
#define PITCH_BEND_CENTER 512
#define PITCH_BEND_UPPER 670
#define PITCH_BEND_SEMITONE_RANGE 12 // in each direction
#define POT_FUZZY 6

#define MIDI_CHANNEL 1 // the midi channel to send to

#define GATE_LED_PIN 12 //gate indicator
#define GATE_PIN 9 //gate control
#define SLAVE_SELECT_PIN 10 //spi chip select

#define DAC_BASE -3000 //-3V offset, TODO: we have to modify this value for tuning, right?
#define DAC_SCALE_PER_SEMITONE 83.333333333
#define GATE_RETRIGGER_DELAY_US 100 //time in microseconds to turn gate off in order to retrigger envelope

MIDI_CREATE_DEFAULT_INSTANCE();


//MIDI variables
int8_t currentVibratoWave = 3; // sine as default
bool currentHold = false;
int8_t currentOctave;
int currentMidiNote; //the note currently being played
int keysPressedArray[128] = {0}; //to keep track of which keys are pressed

int lastSendPitch = 0;

int lastSendMidiPitchBend;                                    
 
const byte ROWS = 4; // four rows
const byte COLS = 7; // seven columns

char keys[ROWS][COLS] = {
  {48, 52, 56, 60, 64, 68, 72},
  {49, 53, 57, 61, 65, 69, 0},
  {50, 54, 58, 62, 66, 70, 0},
  {51, 55, 59, 63, 67, 71, 0}
};

// arduino uno pins (without using MCP23017)
//byte rowPins[ROWS] = {A1, A2, A3, A4};
//byte colPins[COLS] = {2, 3, 4, 5, 6, 7, 8};

// MCP23017 pins
byte rowPins[ROWS] = {
  KBD_R1, KBD_R2, KBD_R3, KBD_R4
};
byte colPins[COLS] = {
  KBD_C1, KBD_C2, KBD_C3, KBD_C4,
  KBD_C5, KBD_C6, KBD_C7
};

int vibratoPotFreqValue = 0;
int vibratoPotAmpValue = 0;


Keypad_MC17 kpd( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR );

lfo lfo_class(256);

bool loobVibratoWaveButtonState = HIGH;
bool holdButtonState = HIGH;
bool octaveButtonDownState = HIGH;
bool octaveButtonUpState = HIGH;


void setup() {
  //Serial.begin(9600); //for debug, can't use midi at the same time!
  Serial.begin(31250); // MIDI baudrate
  while( !Serial ){/*wait*/}   //for USB serial switching boards
  setupKeyboard();
  setupCvOut();
  setupVibrato();
  setupOctaveShift();
  setupHold();
}

void loop() {
  loopKeyPadRead();
  loopVibratoRead();
  loopOctaveButtons();
  loopHoldButton();
  loopVibratoWaveButton();
  setNotePitch(currentMidiNote);
}
