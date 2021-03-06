

#include "libs/lfo/lfo.h"
#include "libs/lfo/lfo.cpp"

#define VIBRATO_LOOP_WAVES_BUTTON_PIN 5 // Arduino Uno pin the momentary switch for eaveform looping is connected to
#define VIBRATO_AMP_PIN A2              // Arduino Uno pin the potentiometer for amplitude is connected to
#define VIBRATO_FREQ_PIN A1             // Arduino Uno pin the potentiometer for frequency is connected to
#define VIBRATO_HZ_LOWER 0.2
#define VIBRATO_HZ_UPPER 13
#define VIBRATO_AMP_LOWER 0
#define VIBRATO_AMP_UPPER 300

// we do not use the full range of the pot as we use the original "mod wheel" unit of AKAI synthstation 25
#define VIBRATO_FREQ_POT_LOWER 367
#define VIBRATO_FREQ_POT_UPPER 670

lfo lfo_class(4096);

int8_t currentVibratoWave = 3; // sine as default
int vibratoPotFreqValue = 0;
int vibratoPotAmpValue = 0;
bool loobVibratoWaveButtonState = HIGH;
bool syncToTempo = true;

void setupVibrato() {

  lfo_class.setWaveForm(currentVibratoWave);   // initialize waveform
  lfo_class.setAmpl(0);       // set amplitude to 0
  lfo_class.setAmplOffset(0); // no offset to the amplitude
  lfo_class.setMode(false);   // set sync mode to mode0 -> no sync to BPM
  lfo_class.setMode0Freq(0);  // set LFO to 0 Hz

  
  //lfo_class.setMode1Bpm(120.0);
  //lfo_class.setMode(true);   // set sync mode to mode0 -> no sync to BPM
  //lfo_class.setMode1Rate(0.25); 
  
  pinMode (VIBRATO_LOOP_WAVES_BUTTON_PIN, INPUT_PULLUP);
  setLedForVibratoWave(currentVibratoWave);
}

float setVibratoFreqFromPotentiometer() {
  // read potentiometer value and debounce
  int x = 0.3 * analogRead(VIBRATO_FREQ_PIN) + 0.7 * vibratoPotFreqValue;
  if (x < VIBRATO_FREQ_POT_LOWER + POT_FUZZY) {
    x = VIBRATO_FREQ_POT_LOWER + POT_FUZZY;
  }
  if (x > VIBRATO_FREQ_POT_UPPER - POT_FUZZY) {
    x = VIBRATO_FREQ_POT_UPPER - POT_FUZZY;
  }
  if (x == vibratoPotFreqValue) {
    // no change since we last set the frequency 
    return;
  }
  vibratoPotFreqValue = x;
  int newFreq = map(
    vibratoPotFreqValue,
    VIBRATO_FREQ_POT_LOWER + POT_FUZZY,
    VIBRATO_FREQ_POT_UPPER - POT_FUZZY,
    VIBRATO_HZ_LOWER*1000,
    VIBRATO_HZ_UPPER*1000
  );
  float newFreqFloat = (float)newFreq/1000;
  lfo_class.setMode0Freq(
    newFreqFloat,
    micros()
  );
  //Serial.print("VIBRATO FREQ ");
  //Serial.println(lfo_class.getMode0Freq());
}

void loopVibratoRead() {

  setVibratoFreqFromPotentiometer();
  int x = 0.3 * analogRead(VIBRATO_AMP_PIN) + 0.7 * vibratoPotAmpValue;
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



void loopVibratoWaveButton() {
  if (digitalRead(VIBRATO_LOOP_WAVES_BUTTON_PIN) == loobVibratoWaveButtonState) {
    // button state did not change
    return;
  }
  loobVibratoWaveButtonState = !loobVibratoWaveButtonState;
  if (loobVibratoWaveButtonState == HIGH) {
    // button has been released
    return;
  }
  // button has been pressed
  currentVibratoWave++;
  // 0 off
  // 1 saw
  // 2 triangle
  // 3 sine
  // 4 square
  if (currentVibratoWave == 5) {
    currentVibratoWave = 1;
  }
  // Serial.println(" ");
  // Serial.print("set wave to ");
  // Serial.println(currentVibratoWave);
  lfo_class.setWaveForm(currentVibratoWave);
  setLedForVibratoWave(currentVibratoWave);
}
