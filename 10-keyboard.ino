 

// keyboard unit's row/col pins are connected to an MCP23017
#include <Keypad.h>
#include "libs/keypad-MCP23017/Keypad_MC17.h"
#include "libs/keypad-MCP23017/Keypad_MC17.cpp"

#define I2CADDR 0x20        // address of MCP23017 chip on I2C bus

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

int keysPressedArray[128] = {0}; //to keep track of which keys are pressed

Keypad_MC17 kpd( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR );



void setupKeyboard() {
  Wire.begin( );
  kpd.begin( );
}

void loopKeyboardRead() {
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
        handleKeyboardNotePressed((kpd.key[i].kchar - '0') + 48);
        break;

      case RELEASED:
        handleKeyboardNoteReleased((kpd.key[i].kchar - '0') + 48);
        break;
    }
  }
}


// one of the 25 keys has been pressed
void handleKeyboardNotePressed(byte pitch) { 
  // this function is called automatically when a note on message is received 
  keysPressedArray[pitch] = 1;
  synthNoteOn(pitch);
  handleKeyboardNotePressedMidi(pitch);
}

// one of the 25 keys has been released
void handleKeyboardNoteReleased(byte pitch)
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
  handleKeyboardNoteReleasedMidi(pitch);
  
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
  cvGateOff();
  delayMicroseconds(GATE_RETRIGGER_DELAY_US); //should not do delays here really but get away with this which seems to be the minimum a montotron needs (may be different for other synths)
  cvGateOn();
  currentMidiNote = note; //store the current note

  //Serial.print("pitch ");
  //Serial.println(note + currentOctave * 12);
  
}

void synthNoteOff(int note) {
  //Serial.print("Key ");
  //Serial.print(note);
  //Serial.println(" OFF");
  if (currentHold == true) {
    return;
  }
  cvGateOff();
}
