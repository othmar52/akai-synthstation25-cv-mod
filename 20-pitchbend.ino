 
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
