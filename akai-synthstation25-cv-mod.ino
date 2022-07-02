/*
  @see
  https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi-cv.pdf
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi_cv2.ino
*/

#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 7; //three columns
/*
 * -----------------------------------------------------------
 *                       note numbers
 *  octave |  C   C#  D   D#  E   F   F#  G   G#  A   A#  B
 * -----------------------------------------------------------
 *    -2   |  0   1   2   3   4   5   6   7   8   9   10  11
 *    -1   |  12  13  14  15  16  17  18  19  20  21  22  23
 *     0   |  24  25  26  27  28  29  30  31  32  33  34  35
 *     1   |  36  37  38  39  40  41  42  43  44  45  46  47
 *     2   |  48  49  50  51  52  53  54  55  56  57  58  59
 *     3   |  60  61  62  63  64  65  66  67  68  69  70  71
 *     4   |  72  73  74  75  76  77  78  79  80  81  82  83
 *     5   |  84  85  86  87  88  89  90  91  92  93  94  95
 *     6   |  96  97  98  99  100 101 102 103 104 105 106 107
 *     7   |  108 109 110 111 112 113 114 115 116 117 118 119
 *     8   |  120 121 122 123 124 125 126 127
 */


/*

Arduino SPI pins
10: CS/SS
11: MOSI (data output)  <- pretty sure we have to use this one
12: MISO (data input)
13: CLOCK

*/                                        
 
char keys[ROWS][COLS] = {
  {48, 52, 56, 60, 64, 68, 72},
  {49, 53, 57, 61, 65, 69, 0},
  {50, 54, 58, 62, 66, 70, 0},
  {51, 55, 59, 63, 67, 71, 0}
};

byte rowPins[ROWS] = {A1, A2, A3, A4};
byte colPins[COLS] = {2, 3, 4, 5, 6, 7, 8};

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

String msg;


void setup() {
  Serial.begin(9600);
  msg = "";
}

void loop() {
  loopKeyPadRead();
}

void loopKeyPadRead() {
  if (!kpd.getKeys())
  {
    return;
  }
  for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
  {
    if ( !kpd.key[i].stateChanged )   // Only find keys that have changed state.
    {
      continue;
    }
    switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED

      case HOLD:
      case IDLE:
        continue;
      case PRESSED:
        msg = " PRESSED.";
        break;

      case RELEASED:
        msg = " RELEASED.";
        break;
    }
    Serial.print("Key ");
    Serial.print((kpd.key[i].kchar - '0') + 48);
    //Serial.print(kpd.key[i].kchar);
    Serial.println(msg);
  }
}
