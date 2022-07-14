#define NUMBATONPIXELS 16
#define BATONPIXELSTART 0

const int countDownToQuarterNote = 64; /** end of countdown [quarter note] (should be a multiple of NUMBATONPIXELS) */
const int ppqn = 4;                                /** pulses per quarter note */
const int maxTicks = ppqn * countDownToQuarterNote; /** when to reset tick counter to zero */
const int countDownStartTick = ppqn * (countDownToQuarterNote - 4);


int insideQuarterNoteCounter = 0;  /** on quarter note start */
int currentSection = 1;            /** loop index (starting from 1) */
int currentStepLedIndex = 0;       /** index of last highlighted led (starting from 0) */
const int numSections = countDownToQuarterNote / NUMBATONPIXELS; /** amount of loops before restart */

uint32_t tickCounterBatonLoop = 0; /** incremental counter since clock-start-event */

void blazingBatonStart()
{
  tickCounterBatonLoop = tickCounter % maxTicks;
  insideQuarterNoteCounter = (tickCounterBatonLoop / ppqn) + 1;
  prepareNewLedStates();
  checkLedChange();
}

void blazingBatonStop()
{
  tickCounterBatonLoop = tickCounter % maxTicks;
  insideQuarterNoteCounter = (tickCounterBatonLoop / ppqn) + 1;
  prepareNewLedStates();
  checkLedChange();
}

void blazingBatonClockTick()
{
  tickCounterBatonLoop = tickCounter % maxTicks;
  insideQuarterNoteCounter = (tickCounterBatonLoop / ppqn) + 1;
  currentSection = (tickCounterBatonLoop / (ppqn * NUMBATONPIXELS)) + 1;
  prepareNewLedStates();
  checkLedChange();
}

/** this method does not trigger any led change but sets the stats in the newArray variable */
void prepareNewLedStates()
{
  if (clockRunning == false)
  {
    idleLedStates();
    return;
  }
  if (tickCounterBatonLoop < countDownStartTick)
  {
    standardLedStates();
    return;
  }
  countdownLedStates();
}

/**
 * clock is not running
 * alternating blink of 1st and 2nd led in quarter note interval
 * TODO: this does not work in case we do not receive clock ticks
 */
void idleLedStates()
{
  clearBatonLedStates();
  if (insideQuarterNoteCounter % 2 == 0)
  {
    newState[BATONPIXELSTART] = getColorDefault();
    return;
  }
  newState[(BATONPIXELSTART+1)] = getColorDefault();
}

/**
 * turn off all blazing baton leds
 * but dont change all other leds
 */
void clearBatonLedStates()
{
  batchColor(1, 16, 0);
}

/**
 * increment colored leds by one on each quarter note change
 * and start from 1 again when we reach the last led
 */
void standardLedStates()
{
  currentStepLedIndex = 0;
  for (int i = 0; i < NUMBATONPIXELS; i++)
  {
    newState[(i+BATONPIXELSTART)] = 0;
    if (i + 1 <= insideQuarterNoteCounter % NUMBATONPIXELS || insideQuarterNoteCounter % NUMBATONPIXELS == 0)
    {
      newState[(i+BATONPIXELSTART)] = getColorDefault();
      currentStepLedIndex = i;
    }
  }
  sectionIndexLedStates();
}

uint32_t getColorDefault() {
  return colorMagenta;
}
uint32_t getColorSectionIndex() {
  return colorCyan;
}
uint32_t getColorCountDown() {
  return colorBlue;
}

/**
 * perma lightning the last few leds depending on loop index before starting over again
 * but current step has a higher priority
 */
void sectionIndexLedStates()
{
  int ledIndexOfSection;
  for (int i = 0; i < currentSection; i++)
  {
    ledIndexOfSection = NUMBATONPIXELS - numSections + i;
    if (ledIndexOfSection == currentStepLedIndex)
    {
      continue;
    }
    newState[(ledIndexOfSection+BATONPIXELSTART)] = getColorSectionIndex();
  }
}

/**
 * NOTE: led index arguments are starting from 1 (but array indices starts from zero)
 */
void batchColor(int ledIndexFrom, int ledIndexTo, uint32_t color)
{
  for (int i = ledIndexFrom; i <= ledIndexTo; i++)
  {
    newState[(i - 1 + BATONPIXELSTART)] = color;
  }
}
