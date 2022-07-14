



#define OCTAVE_DOWN_BUTTON_PIN 6
#define OCTAVE_UP_BUTTON_PIN 8
#define OCTAVE_SHIFT_RANGE 2 // 2 octaves up, 2 octaves down


#define OCTAVE_PIN A3


bool octaveButtonDownState = HIGH;
bool octaveButtonUpState = HIGH;


void setupOctaveShift() {
  pinMode (OCTAVE_DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode (OCTAVE_UP_BUTTON_PIN, INPUT_PULLUP);
  setLedForCurrentOctave(currentOctave);
}

void loopOctaveButtons() {
  if (digitalRead(OCTAVE_DOWN_BUTTON_PIN) != octaveButtonDownState) {
    octaveButtonDownState = !octaveButtonDownState;
    if (octaveButtonDownState == LOW) {
      if (octaveButtonUpState == LOW) {
        resetOctave();
      } else {
        octaveDown();
      }
    }
  }
  if (digitalRead(OCTAVE_UP_BUTTON_PIN) != octaveButtonUpState) {
    octaveButtonUpState = !octaveButtonUpState;
    if (octaveButtonUpState == LOW) {
      if (octaveButtonDownState == LOW) {
        resetOctave();
      } else {
        octaveUp();
      }
    }
  }
}

void octaveUp() {
  currentOctave++;
  if (currentOctave > OCTAVE_SHIFT_RANGE) {
    currentOctave = OCTAVE_SHIFT_RANGE;
  }
  setLedForCurrentOctave(currentOctave);
}
void octaveDown() {
  currentOctave--;
  if (currentOctave < OCTAVE_SHIFT_RANGE * -1) {
    currentOctave = OCTAVE_SHIFT_RANGE * -1;
  }
  setLedForCurrentOctave(currentOctave);
}
void resetOctave() {
  currentOctave = 0;
  setLedForCurrentOctave(currentOctave);
}
