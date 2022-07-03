/*
  @see
  https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi-cv.pdf
  https://github.com/petegaggs/MIDI-to-CV/blob/master/midi_cv2.ino
*/

#include <Keypad.h>

// include the SPI library:
#include <SPI.h>


#define GATE_PIN 9 //gate control
#define SLAVE_SELECT_PIN 10 //spi chip select

#define DAC_BASE -3000 //-3V offset 
#define DAC_SCALE_PER_SEMITONE 83.333333333
#define GATE_RETRIGGER_DELAY_US 100 //time in microseconds to turn gate off in order to retrigger envelope

//MIDI variables
int currentMidiNote; //the note currently being played
int keysPressedArray[128] = {0}; //to keep track of which keys are pressed

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

  //SPI stuff
  // set the slaveSelectPin as an output:
  pinMode (SLAVE_SELECT_PIN, OUTPUT);
  digitalWrite(SLAVE_SELECT_PIN,HIGH); //set chip select high
  // initialize SPI:
  SPI.begin(); 
  //Serial.begin(9600); //for debug, can't use midi at the same time!
  pinMode (GATE_PIN, OUTPUT); //gate for monotron
  digitalWrite(GATE_PIN,LOW); //turn note off
  //dacWrite(1000); //set the pitch just for testing
  
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
        handleNoteOn((kpd.key[i].kchar - '0') + 48);
        break;

      case RELEASED:
        handleNoteOff((kpd.key[i].kchar - '0') + 48);
        break;
    }
  }
}

void dacWrite(int value) {
  //write a 12 bit number to the MCP8421 DAC
  if ((value < 0) || (value > 4095)) {
    value = 0;
  }
  // take the SS pin low to select the chip:
  digitalWrite(SLAVE_SELECT_PIN,LOW);
  //send a value to the DAC
  SPI.transfer(0x10 | ((value >> 8) & 0x0F)); //bits 0..3 are bits 8..11 of 12 bit value, bits 4..7 are control data 
  SPI.transfer(value & 0xFF); //bits 0..7 of 12 bit value
  // take the SS pin high to de-select the chip:
  digitalWrite(SLAVE_SELECT_PIN,HIGH); 
}


void setNotePitch(int note) {
  //receive a midi note number and set the DAC voltage accordingly for the pitch CV
  dacWrite(DAC_BASE+(note*DAC_SCALE_PER_SEMITONE)); //set the pitch of the oscillator
}


void handleNoteOn(byte pitch) { 
  // this function is called automatically when a note on message is received 
  keysPressedArray[pitch] = 1;
  synthNoteOn(pitch);
}

void handleNoteOff(byte pitch)
{
  keysPressedArray[pitch] = 0; //update the array holding the keys pressed 
  if (pitch == currentMidiNote) {
    //only act if the note released is the one currently playing, otherwise ignore it
    int highestKeyPressed = findHighestKeyPressed(); //search the array to find the highest key pressed, will return -1 if no keys pressed
    if (highestKeyPressed != -1) { 
      //there is another key pressed somewhere, so the note off becomes a note on for the highest note pressed
      synthNoteOn(highestKeyPressed);
    }    
    else  {
      //there are no other keys pressed so proper note off
      synthNoteOff(pitch);
    }
  }  
}

int findHighestKeyPressed(void) {
  //search the array to find the highest key pressed. Return -1 if no keys are pressed
  int highestKeyPressed = -1; 
  for (int count = 0; count < 127; count++) {
    //go through the array holding the keys pressed to find which is the highest (highest note has priority), and to find out if no keys are pressed
    if (keysPressedArray[count] == 1) {
      highestKeyPressed = count; //find the highest one
    }
  }
  return(highestKeyPressed);
}

void synthNoteOn(int note) {
  Serial.print("Key ");
  Serial.print(note);
  Serial.println(" ON");
  //starts playback of a note
  setNotePitch(note); //set the oscillator pitch
  digitalWrite(GATE_PIN, LOW); //turn gate off momentarily to retrigger LFO
  delayMicroseconds(GATE_RETRIGGER_DELAY_US); //should not do delays here really but get away with this which seems to be the minimum a montotron needs (may be different for other synths)
  digitalWrite(GATE_PIN,HIGH); //turn gate on
  currentMidiNote = note; //store the current note
}

void synthNoteOff(int note) {
  Serial.print("Key ");
  Serial.print(note);
  Serial.println(" OFF");
  digitalWrite(GATE_PIN, LOW); //turn gate off
}
