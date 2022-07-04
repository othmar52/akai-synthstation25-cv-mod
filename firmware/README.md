# Arduino as a class compliant USB MIDI device

In case you have **ATMEGA16U2** chip on your Arduino you can turn it into a class compliant USB MIDI device.  
So there will be no need to build DIN MIDI in + out plugs.

Many thanks to [Nerd Musician](https://www.youtube.com/channel/UCyqCwyBJ98fR-CPoyXUxY5w) for this tutorial  
https://www.youtube.com/watch?v=18OKo9sQ_s0  


Arduino dual boot firmware `arduino-akai-25-eurorack.hex` created via  
https://moco-lufa-web-client.herokuapp.com/#/
```
 Device Name: 25-eurorack
Manufacturer: AKAI
         PID: 0x7059
```

```
sudo dfu-programmer atmega16u2 erase
sudo dfu-programmer atmega16u2 flash arduino-akai-25-eurorack.hex 
sudo dfu-programmer atmega16u2 reset
```
