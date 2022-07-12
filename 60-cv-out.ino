 
void setupCvOut() {
  pinMode (GATE_LED_PIN, OUTPUT); //gate indicator
  pinMode (GATE_PIN, OUTPUT); //gate CV
  cvGateOff();
  //dacWrite(1000); //set the pitch just for testing
}

void setNotePitch(int note) {
  //receive a midi note number and set the DAC voltage accordingly for the pitch CV
  int dacValue = DAC_BASE+(note*DAC_SCALE_PER_SEMITONE)+ getPitchbendOffset() + getOctaveOffset();


  if (vibratoPotAmpValue > 0) {
    lfo_class.setAmplOffset(dacValue/16);
    dacWrite(lfo_class.getWave(micros())*16);
    return;
  }
  dacWrite(dacValue);
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


void cvGateOff() {
  digitalWrite(GATE_PIN, LOW);
  digitalWrite(GATE_LED_PIN,LOW);
}

void cvGateOn() {
  digitalWrite(GATE_PIN, HIGH);
  digitalWrite(GATE_LED_PIN,HIGH);
}
