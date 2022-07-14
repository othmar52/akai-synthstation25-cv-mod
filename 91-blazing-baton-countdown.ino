
/**
 * countdownLedStates()
 * led state of the last 4 quarter notes of 16 bars
 **********************************************
 *   3   X X X X  - - X X  X X - -  X X X X
 *   2   - - X X  X X - -  - - X X  X X - -
 *   1   - - - -  - - X X  X X - -  - - - -
 *  and  X X X X  X X X X  X X X X  X X X X
 **********************************************
 */
void countdownLedStates()
{
  const int flashDuration = 23; /** time in [ticks] before turning leds off */

  switch (tickCounterBatonLoop)
  {
  case countDownStartTick:
    clearBatonLedStates();
    batchColor(1, 4, getColorCountDown());
    batchColor(7, 10, getColorCountDown());
    batchColor(13, 16, getColorCountDown());
    break;
  case countDownStartTick + ppqn:
    clearBatonLedStates();
    batchColor(3, 6, getColorCountDown());
    batchColor(11, 14, getColorCountDown());
    break;
  case countDownStartTick + 2 * ppqn:
    clearBatonLedStates();
    batchColor(7, 10, getColorCountDown());
    break;
  case countDownStartTick + 3 * ppqn:
    batchColor(1, 16, getColorCountDown());
    break;
  case countDownStartTick + flashDuration:
  case countDownStartTick + flashDuration + ppqn:
  case countDownStartTick + flashDuration + 2 * ppqn:
  case maxTicks - 1:
    clearBatonLedStates();
    break;
  }
}
