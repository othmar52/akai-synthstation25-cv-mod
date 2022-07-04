# AKAI Synthstation 25 CV MOD for eurorack
How to modify the tiny [AKAI Synthstation 25 keyboard](https://www.akaipro.com/synthstation25) for usage in eurorack/control voltage.  

![image](https://user-images.githubusercontent.com/7056051/177115348-58fd0b93-875e-407c-8ef1-f70a4646167e.png)  


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
───────────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
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

see my other [eurorack DIY projects](https://github.com/othmar52/eurorack)  

