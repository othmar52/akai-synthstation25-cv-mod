 

/*

Arduino Uno SPI pins
10: CS/SS               <- we have to use this one for the MCP4821 CS/SS pin
11: MOSI (data output)  <- we have to use this one for the MCP4821 SDI pin
12: MISO (data input)
13: CLOCK               <- we have to use this one for the MCP4821 SCK pin

*/


void setupKeyboard() {
  Wire.begin( );
  kpd.begin( );
  //SPI stuff
  // set the slaveSelectPin as an output:
  pinMode (SLAVE_SELECT_PIN, OUTPUT);
  digitalWrite(SLAVE_SELECT_PIN,HIGH); //set chip select high
  // initialize SPI:
  SPI.begin(); 
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
