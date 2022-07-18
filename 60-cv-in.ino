 

#define CLOCK_INPUT_PIN 2 // CV clock signal input pin, must be usable for interrupts
#define RESET_INPUT_PIN 3 // CV reset signal input pin, must be usable for interrupts

// those variable may be useful as soon as we implement additional features (e. g. arp)
bool clockRunning = false;
unsigned long tickCounter = 0; // incremental counter since clock-start-event

// calculate the tempo for syncing vibrato
const int PPQN = 4;
float bpm = .0;
unsigned long currentMilliSecond = 0;
unsigned long lastBarStartMilliSecond = 0;


void setupCvClockReset() {
  // Interrupts
  pinMode(CLOCK_INPUT_PIN, INPUT);
  pinMode(RESET_INPUT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INPUT_PIN), isrClock, RISING);
  attachInterrupt(digitalPinToInterrupt(RESET_INPUT_PIN), isrReset, CHANGE);
}

void isrClock() {
  handleEventIncomingCvClockTick();  
}

void isrReset() {
  if (digitalRead(RESET_INPUT_PIN) == LOW) {
    handleEventIncomingCvClockStart();
    return;
  }
  handleEventIncomingCvClockStop();
}

void handleEventIncomingCvClockTick()
{
  tickCounter++;

  if (tickCounter == 1)
  {
    lastBarStartMilliSecond = millis();
  }
  currentMilliSecond = millis();
  if (tickCounter % PPQN == 0) {
    bpm = 60000.0 / ((float)currentMilliSecond - (float)lastBarStartMilliSecond);
    lastBarStartMilliSecond = currentMilliSecond;
    // Serial.print("  BPM: ");
    // Serial.println(bpm);
  }
  // do stuff thats only blazing baton related
  blazingBatonClockTick();
}


void handleEventIncomingCvClockStart()
{
  clockRunning = true;
  tickCounter = 0;
  // do stuff thats only blazing baton related
  blazingBatonStart();
}

void handleEventIncomingCvClockStop()
{
  clockRunning = false;
  tickCounter = 0;
  // do stuff thats only blazing baton related
  blazingBatonStop();
}
