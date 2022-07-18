# AKAI Synthstation 25 CV MOD for eurorack
How to modify the tiny [AKAI Synthstation 25 keyboard](https://www.akaipro.com/synthstation25) for usage in eurorack/control voltage.  

![image](https://user-images.githubusercontent.com/7056051/177115348-58fd0b93-875e-407c-8ef1-f70a4646167e.png)  

![image](https://user-images.githubusercontent.com/7056051/178199092-979addfc-fe6c-423c-9a65-7714f91269df.png)  

![image](https://user-images.githubusercontent.com/7056051/178199271-bdeffb37-29a0-4015-8dd5-f643f93abbb5.png)  



## Akai Synth station 25: 16 pin flat cable of keyboard unit
```
pin numers on PCB starts with 1 on the very right (top view of keyboard)
───────────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
   Keyboard│    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │
 (top view)│ 16 │ 15 │ 14 │ 13 │ 12 │ 11 │ 10 │  9 │  8 │  7 │  6 │  5 │  4 │  3 │  2 │  1 │
 flat cable│    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │
───────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
    row/col│    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │
     Keypad│    │ C7 │ C6 │ C5 │ C4 │ C3 │ C2 │ C1 │    │ R4 │    │ R3 │    │ R2 │    │ R1 │
    library│    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │
───────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
    arduino│    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │
    uno pin│    │  8 │  7 │  6 │  5 │  4 │  3 │  2 │    │ A4 │    │ A3 │    │ A2 │    │ A1 │
 connection│    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │
───────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
         or│    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │
   MCP23017│    │ 10 │  9 │  8 │  7 │  6 │  5 │  4 │    │  3 │    │  2 │    │  1 │    │  0 │
 connection│    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │    │
───────────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
```

## MCP23017 pins
```

      7   6   5   4   3   2   1   0          3V  GND GND GND
      |   |   |   |   |   |   |   |   |   |   |   |   |   |
    ┌───────────────────────────────────────────────────────┐
    │                                                       │
    Ↄ                  MCP23017 TOP VIEW                    │
    │                                                       │
    └───────────────────────────────────────────────────────┘
      |   |   |   |   |   |   |   |   |   |   |   |   |   |
      8   9  10  11  12  13  14  15  3V  GND      A5  A4

```

## MCP4821 (12-Bit Single Output DAC with SPI)
for sending pitch CV. (thanks to https://github.com/petegaggs/MIDI-to-CV/blob/master/midi-cv.pdf)
```
                                  MCP4821
                                 top view
                              _______ _______
                             |       U       |
 to MCP4821 pin 6 (SHDN) <- -| 1 VDD  VOUT 8 |- -> 100 R -> PITCH CV JACK
   to Arduino Uno pin 10 <- -| 2 CS    VSS 7 |- -> GND
   to Arduino Uno pin 13 <- -| 3 SCK  SHDN 6 |- -> 5V (with 10 uF cap to GND)
   to Arduino Uno pin 11 <- -| 4 SDI  LDAC 5 |- -> GND
                             |_______________|

```
## Gate indicator LED
```
short leg -> Arduino Uno GND
 long leg -> resistor 220 R -> Arduino Uno pin 12
```

## some notes for building the prototype PCB
```
those arduino Arduino Uno pins seems to be not changeable
A4 + A5 to MCP23017
2 CLOCK_INPUT_PIN (interrupts support needed)
3 RESET_INPUT_PIN (interrupts support needed)
10 MCP4821 CS
11 MCP4821 SDI
13 MCP4821 SDK



any analog pin for the potentiometers
#define VIBRATO_AMP_PIN A2
#define VIBRATO_FREQ_PIN A1
#define PITCH_BEND_PIN A0
#define OCTAVE_PIN A3

any digital pin for momentary switches 
#define OCTAVE_DOWN_BUTTON_PIN 6
#define OCTAVE_UP_BUTTON_PIN 8
#define VIBRATO_LOOP_WAVES_BUTTON_PIN 5
#define HOLD_BUTTON_PIN 4
GATE_PIN 9
7 PINLEDSTRIP 
```



see my other [eurorack DIY projects](https://github.com/othmar52/eurorack)  

