/*
  @see
  https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi-cv.pdf
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi_cv2.ino

  https://github.com/joeyoung/arduino_keypads/tree/master/Keypad_MC17
  
*/

#include <Keypad.h>
#include "Keypad_MC17.h"
#include <MIDI.h>

#include "lfo/lfo.h"
#include "lfo/lfo.cpp"

// include the SPI library:
#include <SPI.h>

#define I2CADDR 0x20        // address of MCP23017 chip on I2C bus

#define DACSIZE 4096

#define VIBRATO_AMP_PIN A2  // Arduino Uno pin the amplitude potentiometer is connected to
#define VIBRATO_FREQ_PIN A3 // Arduino Uno pin the frequency potentiometer is connected to
#define VIBRATO_HZ_LOWER 0.2
#define VIBRATO_HZ_UPPER 13
#define VIBRATO_AMP_LOWER 0
#define VIBRATO_AMP_UPPER 300


#define OCTAVE_PIN A1
#define PITCH_BEND_PIN A0 // pitch bend potentiometer
#define PITCH_BEND_CENTER 512
#define PITCH_BEND_UPPER 670
#define PITCH_BEND_LOWER 360
#define PITCH_BEND_SEMITONE_RANGE 12 // in each direction
#define POT_FUZZY 6

#define MIDI_CHANNEL 1 // the midi channel to send to

#define GATE_LED_PIN 12 //gate indicator
#define GATE_PIN 9 //gate control
#define SLAVE_SELECT_PIN 10 //spi chip select

#define DAC_BASE -3000 //-3V offset 
#define DAC_SCALE_PER_SEMITONE 83.333333333
#define GATE_RETRIGGER_DELAY_US 100 //time in microseconds to turn gate off in order to retrigger envelope

MIDI_CREATE_DEFAULT_INSTANCE();


//MIDI variables
int currentOctave;
int currentMidiNote; //the note currently being played
int keysPressedArray[128] = {0}; //to keep track of which keys are pressed

int lastSendPitch = 0;

int lastSendMidiPitchBend;
/*
 * -----------------------------------------------------------
 *                       note numbers
 *  octave |  C   C#  D   D#  E   F   F#  G   G#  A   A#  B
 * -----------------------------------------------------------
 *    -2   |  0   1   2   3   4   5   6   7   8   9   10  11
 *    -1   |  12  13  14  15  16  17  18  19  20  21  22  23
 *     0   |  24  25  26  27  28  29  30  31  32  33  34  35
 *     1   |  36  37  38  39  40  41  42  43  44  45  46  47
 *     2   |  48  49  50  51  52  53  54  55  56  57  58  59
 *     3   |  60  61  62  63  64  65  66  67  68  69  70  71
 *     4   |  72  73  74  75  76  77  78  79  80  81  82  83
 *     5   |  84  85  86  87  88  89  90  91  92  93  94  95
 *     6   |  96  97  98  99  100 101 102 103 104 105 106 107
 *     7   |  108 109 110 111 112 113 114 115 116 117 118 119
 *     8   |  120 121 122 123 124 125 126 127
 */


/*

Arduino Uno SPI pins
10: CS/SS               <- we have to use this one for the MCP4821 CS/SS pin
11: MOSI (data output)  <- we have to use this one for the MCP4821 SDI pin
12: MISO (data input)
13: CLOCK               <- we have to use this one for the MCP4821 SCK pin

*/                                        
 
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
byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7, 8, 9, 10};

int vibratoPotFreqValue = 0;
int vibratoPotAmpValue = 0;


Keypad_MC17 kpd( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR );

lfo lfo_class(DACSIZE);

String msg;


void setup() {
  //Serial.begin(9600);
  Serial.begin(31250); // MIDI baudrate
  while( !Serial ){/*wait*/}   //for USB serial switching boards
  msg = "";
  Wire.begin( );
  kpd.begin( );
  //SPI stuff
  // set the slaveSelectPin as an output:
  
  // pinMode (PICH_BEND_PIN, INPUT);
  pinMode (SLAVE_SELECT_PIN, OUTPUT);
  digitalWrite(SLAVE_SELECT_PIN,HIGH); //set chip select high
  // initialize SPI:
  SPI.begin(); 
  //Serial.begin(9600); //for debug, can't use midi at the same time!
  pinMode (GATE_LED_PIN, OUTPUT); //gate indicator
  pinMode (GATE_PIN, OUTPUT); //gate CV
  digitalWrite(GATE_PIN,LOW); //turn note off
  digitalWrite(GATE_LED_PIN,LOW); //turn led off
  //dacWrite(1000); //set the pitch just for testing
  setupVibrato();
}

void setupVibrato() {

  lfo_class.setWaveForm(3);   // initialize waveform
  lfo_class.setAmpl(0);       // set amplitude to 0
  lfo_class.setAmplOffset(0); // no offset to the amplitude
  lfo_class.setMode(false);   // set sync mode to mode0 -> no sync to BPM
  lfo_class.setMode0Freq(0);  // set LFO to 0 Hz
}

void loopVibratoRead() {
  int x = 0.3 * analogRead(VIBRATO_FREQ_PIN) + 0.7 * vibratoPotFreqValue;
  if (x != vibratoPotFreqValue) {
    vibratoPotFreqValue = x;
    int newFreq = map(
      vibratoPotFreqValue,
      0,
      1024,
      VIBRATO_HZ_LOWER*1000,
      VIBRATO_HZ_UPPER*1000
    );
    float newFreqFloat = (float)newFreq/1000;
    lfo_class.setMode0Freq(
      newFreqFloat
    );
    //Serial.print("VIBRATO FREQ ");
    //Serial.println(lfo_class.getMode0Freq());
  }

  x = 0.3 * analogRead(VIBRATO_AMP_PIN) + 0.7 * vibratoPotAmpValue;
  if (x != vibratoPotAmpValue) {
    vibratoPotAmpValue = x;
    lfo_class.setAmpl(
      map(
        vibratoPotAmpValue,
        0,
        1024,
        VIBRATO_AMP_LOWER,
        VIBRATO_AMP_UPPER
      )
    );
    //Serial.print("VIBRATO AMP ");
    //Serial.println(lfo_class.getAmpl());
  }
}

void loop() {
  loopKeyPadRead();
  loopVibratoRead();
  setNotePitch(currentMidiNote);
}

float getOctaveOffset() {

  int newOctave = getActiveOctave();
  if (currentOctave != newOctave) {
    //Serial.print("new octave = ");
    //Serial.println(newOctave);
    currentOctave = newOctave;
  }
  return DAC_SCALE_PER_SEMITONE * currentOctave * 12;
}

int getActiveOctave() {
  int maxOctaves = 2; // 2 up, 2 down
  int potVal = analogRead(OCTAVE_PIN);
  int newOctave = 0;

  int distance = 1024 / (2 * maxOctaves + 1);
  for (int i = 0; i < maxOctaves; i++) {
    if (potVal < 512 - (i+1) * distance && newOctave != (i+1) * -1) {
      newOctave = (i+1) * -1;
    }
    if (potVal > 512 + (i+1) * distance && newOctave != (i+1)) {
      newOctave = (i+1);
    }
  }
  return newOctave;
}

void sendMidiPitchBend(int pitchValue) {
  if (lastSendMidiPitchBend == pitchValue) {
    return;  
  }
  MIDI.sendPitchBend(pitchValue, MIDI_CHANNEL);
  lastSendMidiPitchBend = pitchValue;
}

float getPitchbendOffset() {
  int potVal = analogRead(PITCH_BEND_PIN);
  int midiPitchBendVal = 64;
  float percent = 0.;
  if (potVal > PITCH_BEND_CENTER - POT_FUZZY && potVal < PITCH_BEND_CENTER + POT_FUZZY) {
    sendMidiPitchBend(midiPitchBendVal);
    return percent;
  }
  if (potVal < PITCH_BEND_CENTER) {
    if (potVal < PITCH_BEND_LOWER + POT_FUZZY) {
      potVal = PITCH_BEND_LOWER + POT_FUZZY;
    }
    percent = map(potVal, PITCH_BEND_LOWER + POT_FUZZY, PITCH_BEND_CENTER - POT_FUZZY, -100, 0);
    midiPitchBendVal = map(potVal, PITCH_BEND_LOWER + POT_FUZZY, PITCH_BEND_CENTER - POT_FUZZY, 0, 63);
    ////Serial.println("pitch down");
  } else {
    if (potVal > PITCH_BEND_UPPER - POT_FUZZY) {
      potVal = PITCH_BEND_UPPER - POT_FUZZY;
    }
    percent = map(potVal, PITCH_BEND_CENTER + POT_FUZZY, PITCH_BEND_UPPER - POT_FUZZY, 0, 100);
    midiPitchBendVal = map(potVal, PITCH_BEND_CENTER + POT_FUZZY, PITCH_BEND_UPPER - POT_FUZZY, 65, 127);
    ////Serial.println("pitch up");
  }
  sendMidiPitchBend(midiPitchBendVal);
  percent = percent * (DAC_SCALE_PER_SEMITONE*PITCH_BEND_SEMITONE_RANGE/100);
  ////Serial.println(percent);
  return percent;
}

void loopKeyPadRead() {
  if (!kpd.getKeys())
  {
    return;
  }
  for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
  {
    if ( !kpd.key[i].stateChanged )   // Only find keys that have changed state.
    {
      continue;
    }
    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED

      case HOLD:
      case IDLE:
        continue;
      case PRESSED:
        handleNoteOn((kpd.key[i].kchar - '0') + 48);
        break;

      case RELEASED:
        handleNoteOff((kpd.key[i].kchar - '0') + 48);
        break;
    }
  }
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


void setNotePitch(int note) {
  //receive a midi note number and set the DAC voltage accordingly for the pitch CV
  int dacValue = DAC_BASE+(note*DAC_SCALE_PER_SEMITONE)+ getPitchbendOffset() + getOctaveOffset();


  if (vibratoPotAmpValue > 0) {
    lfo_class.setAmplOffset(dacValue);
    dacWrite(lfo_class.getWave(micros()));
    return;
  }
  dacWrite(dacValue);
}


void handleNoteOn(byte pitch) { 
  // this function is called automatically when a note on message is received 
  keysPressedArray[pitch] = 1;
  synthNoteOn(pitch);
  MIDI.sendNoteOn(pitch + currentOctave * 12, 127, MIDI_CHANNEL);
}

void handleNoteOff(byte pitch)
{
  keysPressedArray[pitch] = 0; //update the array holding the keys pressed 
  if (pitch == currentMidiNote) {
    //only act if the note released is the one currently playing, otherwise ignore it
    int highestKeyPressed = findHighestKeyPressed(); //search the array to find the highest key pressed, will return -1 if no keys pressed
    if (highestKeyPressed != -1) { 
      //there is another key pressed somewhere, so the note off becomes a note on for the highest note pressed
      synthNoteOn(highestKeyPressed);
    }    
    else  {
      //there are no other keys pressed so proper note off
      synthNoteOff(pitch);
    }
  } 
  MIDI.sendNoteOff(pitch + currentOctave * 12, 0, MIDI_CHANNEL); 
}

int findHighestKeyPressed(void) {
  //search the array to find the highest key pressed. Return -1 if no keys are pressed
  int highestKeyPressed = -1; 
  for (int count = 0; count < 127; count++) {
    //go through the array holding the keys pressed to find which is the highest (highest note has priority), and to find out if no keys are pressed
    if (keysPressedArray[count] == 1) {
      highestKeyPressed = count; //find the highest one
    }
  }
  return(highestKeyPressed);
}

void synthNoteOn(int note) {
  //Serial.print("Key ");
  //Serial.print(note);
  //Serial.println(" ON");
  //starts playback of a note
  setNotePitch(note); //set the oscillator pitch
  digitalWrite(GATE_PIN, LOW); //turn gate off momentarily to retrigger LFO
  delayMicroseconds(GATE_RETRIGGER_DELAY_US); //should not do delays here really but get away with this which seems to be the minimum a montotron needs (may be different for other synths)
  digitalWrite(GATE_PIN,HIGH); //turn gate on
  digitalWrite(GATE_LED_PIN,HIGH);
  currentMidiNote = note; //store the current note

  //Serial.print("pitch ");
  //Serial.println(note + currentOctave * 12);
  
}

void synthNoteOff(int note) {
  //Serial.print("Key ");
  //Serial.print(note);
  //Serial.println(" OFF");
  digitalWrite(GATE_PIN, LOW); //turn gate off
  digitalWrite(GATE_LED_PIN,LOW);
}
