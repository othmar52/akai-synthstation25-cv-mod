# AKAI Synthstaion 25 CV MOD for eurorack
How to modify the tiny [AKAI Synthstaion 25 keyboard](https://www.akaipro.com/synthstation25) for usage in eurorack/control voltage.  



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
