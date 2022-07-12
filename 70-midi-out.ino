 

/*
 * -----------------------------------------------------------
 *                      MIDI note numbers
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

// TODO: close or reopen note when we have an octave shift during noteOn to avoid stuck notes

#include <MIDI.h>

#define MIDI_CHANNEL 1            // the midi channel to send to
MIDI_CREATE_DEFAULT_INSTANCE();
int lastSendMidiPitchBend;

void getPitchBendValueMidi() {
  int potVal = analogRead(PITCH_BEND_PIN);
  int midiPitchBendVal = 64;
  if (pitchBendPercent == 0) {
    return midiPitchBendVal;
  }
  if (pitchBendPercent > 0) {
    return map(potVal, PITCH_BEND_CENTER + POT_FUZZY, PITCH_BEND_UPPER - POT_FUZZY, 65, 127);
  }
  map(potVal, PITCH_BEND_LOWER + POT_FUZZY, PITCH_BEND_CENTER - POT_FUZZY, 0, 63);
}

void handleKeyboardNotePressedMidi(byte pitch) {
  MIDI.sendNoteOn(
    pitch + currentOctave * 12,  // note number
    127,                         // velocity
    MIDI_CHANNEL                 // midi channel
  );
}


void handleKeyboardNoteReleasedMidi(byte pitch) {
  MIDI.sendNoteOff(
    pitch + currentOctave * 12,  // note number
    0,                           // velocity
    MIDI_CHANNEL                 // midi channel
  );
}

void sendMidiPitchBend(int pitchValue) {
  if (lastSendMidiPitchBend == pitchValue) {
    return;  
  }
  MIDI.sendPitchBend(pitchValue, MIDI_CHANNEL);
  lastSendMidiPitchBend = pitchValue;
}
