

#define PITCH_BEND_PIN A0 // pitch bend potentiometer
// we do not use the full range of the pot as we use the original "pitch bend" unit of AKAI synthstation 25
#define PITCH_BEND_LOWER 360
#define PITCH_BEND_CENTER 512
#define PITCH_BEND_UPPER 670
#define PITCH_BEND_SEMITONE_RANGE 12 // in each direction


// based on the potentiometer rotation we can calculte percent positive/negative
// -100 ... 0 ... 100
int8_t pitchBendPercent;

void setupPitchBend() {
  
}

void loopPitchBendPot() {
  int potVal = analogRead(PITCH_BEND_PIN);
  float percent = 0.;
  if (potVal > PITCH_BEND_CENTER - POT_FUZZY && potVal < PITCH_BEND_CENTER + POT_FUZZY) {
    pitchBendPercent = 0.;
    return;
  }
  if (potVal < PITCH_BEND_CENTER) {
    if (potVal < PITCH_BEND_LOWER + POT_FUZZY) {
      potVal = PITCH_BEND_LOWER + POT_FUZZY;
    }
    pitchBendPercent = map(
      potVal,
      PITCH_BEND_LOWER + POT_FUZZY,
      PITCH_BEND_CENTER - POT_FUZZY,
      -100,
      0
    );
    return;
  }

  if (potVal > PITCH_BEND_UPPER - POT_FUZZY) {
    potVal = PITCH_BEND_UPPER - POT_FUZZY;
  }
  pitchBendPercent = map(
    potVal,
    PITCH_BEND_CENTER + POT_FUZZY,
    PITCH_BEND_UPPER - POT_FUZZY,
    0,
    100
  );
}
