 
/**
 *  2 pixels for octave
 *  1 pixel for vibrato wave form
 *  1 pixel for gate out
 *  1 pixel for hold mode
 * 16 pixels for blazing baton
 * ---------------------------
 * 21 pixels total
 * 
 */

#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 36
#define PINLEDSTRIP 7 /** input pin Neopixel is attached to */

#define PIXEL_INDEX_OCTAVE_DOWN 35
#define PIXEL_INDEX_OCTAVE_UP 33
#define PIXEL_INDEX_HOLD 5

#define PIXEL_INDEX_VIBRATO_WAVE 7
#define PIXEL_INDEX_GATE_OUT 2

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PINLEDSTRIP, NEO_GRB + NEO_KHZ800);

const int NUMSTATES = NUMPIXELS + 1;
uint32_t lastState[NUMSTATES];
uint32_t newState[NUMSTATES];


/** TODO: remove unused color vars */
const uint32_t colorWhite = pixels.Color(255, 255, 255);
const uint32_t colorRed = pixels.Color(255, 0, 0);
const uint32_t colorGreen = pixels.Color(0, 255, 0);
const uint32_t colorBlue = pixels.Color(0, 0, 255);
const uint32_t colorCyan = pixels.Color(0, 255, 255);
const uint32_t colorMagenta = pixels.Color(255, 0, 255);
const uint32_t colorYellow = pixels.Color(255, 255, 0);

const uint32_t colorsVibrato[4] = {
  colorBlue,
  colorMagenta,
  colorCyan,
  colorRed
};

void setupLedstrip() {
  initStateArrays();
  setupBlazingBaton();
  pixels.begin(); /** Initializes the NeoPixel library. */
  pixels.clear();
  prepareNewLedStates();
}

void setLedForVibratoWave(uint8_t waveIndex) {
  newState[PIXEL_INDEX_VIBRATO_WAVE] = colorsVibrato[waveIndex-1];
  checkLedChange();
}

void setLedForGateOpen() {
  newState[PIXEL_INDEX_GATE_OUT] = colorMagenta;
  checkLedChange();
}
void setLedForGateClose() {
  newState[PIXEL_INDEX_GATE_OUT] = 0;
  checkLedChange();
}

void setLedForHoldMode(bool holdMode) {
  newState[PIXEL_INDEX_HOLD] = (holdMode == true)
    ? colorRed
    : 0;
  checkLedChange();
}

void setLedForCurrentOctave(int8_t octave) {
  newState[PIXEL_INDEX_OCTAVE_DOWN] = 0;
  newState[PIXEL_INDEX_OCTAVE_UP] = 0;
  switch (octave) {
    case -2:  newState[PIXEL_INDEX_OCTAVE_DOWN] = colorMagenta; break;
    case -1:  newState[PIXEL_INDEX_OCTAVE_DOWN] = colorCyan;    break;
    case  1:  newState[PIXEL_INDEX_OCTAVE_UP]   = colorCyan;    break;
    case  2:  newState[PIXEL_INDEX_OCTAVE_UP]   = colorMagenta; break;
  }
  checkLedChange();
}

void loopLedstrip() {

}


void checkLedChange()
{
  if (hasStateChanged() == false)
  {
    /** nothing to do without any changes within leds */
    return;
  }
  pixels.clear();

  for (int i = 0; i < NUMPIXELS; i++)
  {
    if (newState[i] == 0)
    {
      continue;
    }
    pixels.setPixelColor(i, newState[i]);
  }

  pixels.setBrightness(newState[NUMPIXELS]);
  pixels.show();

  /** persist current led states in lastState array */
  rememberLastState();
}

bool hasStateChanged()
{
  for (int i = 0; i < NUMSTATES; i++)
  {
    if (lastState[i] != newState[i])
    {
      return true;
    }
  }
  return false;
}

/**
 * helper array to check if we need to apply any changes
 * too bad arduino does not support std::array<uint32_t,17> lastState = newState;
 */
void rememberLastState()
{
  for (int i = 0; i < NUMSTATES; i++)
  {
    lastState[i] = newState[i];
  }
}

void clearLedStates()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    newState[i] = 0;
  }
}

/**
 * first <NUMPIXELS> values are led colors; last value is brightness
 */
void initStateArrays()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    newState[i] = 0;
  }
  newState[NUMPIXELS] = 3; // brightness
  rememberLastState();
}
