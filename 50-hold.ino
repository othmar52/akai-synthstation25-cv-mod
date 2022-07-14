 

#define HOLD_BUTTON_PIN 4
 

bool holdButtonState = HIGH;

void setupHold() {
  pinMode (HOLD_BUTTON_PIN, INPUT_PULLUP);
}

void loopHoldButton() {
  if (digitalRead(HOLD_BUTTON_PIN) == holdButtonState) {
    // button state did not change
    return;
  }
  holdButtonState = !holdButtonState;
  if (holdButtonState == HIGH) {
    // button has been released
    return;
  }
  // button has been pressed
  currentHold = !currentHold;
  setLedForHoldMode(currentHold);
  if (currentHold == true) {
    // hold mode has been activated
    return;
  }
  // hold mode has been disabled. close CV gate
  cvGateOff();
  // TODO: send midi all notes off
}
