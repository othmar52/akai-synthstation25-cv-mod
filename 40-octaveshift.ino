
float getOctaveOffset() {
  return DAC_SCALE_PER_SEMITONE * currentOctave * 12;
}
 
void setupOctaveShift() {
  pinMode (OCTAVE_DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode (OCTAVE_UP_BUTTON_PIN, INPUT_PULLUP);
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
}
void octaveDown() {
  currentOctave--;
  if (currentOctave < OCTAVE_SHIFT_RANGE * -1) {
    currentOctave = OCTAVE_SHIFT_RANGE * -1;
  }
}
void resetOctave() {
  currentOctave = 0;
}
