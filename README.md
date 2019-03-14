# ChocolateChip
🍪 An easy to follow CHIP-8 emulator for newcomers into processor design

Some very useful resources:

[Chip 8's Wikipedia Page](https://en.wikipedia.org/wiki/CHIP-8)

[Cowgod's Chip 8 Technical Referece](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

# Reqirements:
* [SFML](https://www.sfml-dev.org) for your platform

* A beep.wav to go in /res/beep.wav

# CHIP 8 Design
The CHIP-8 Has:
* 4KB of Addressable Memory (0x000 - 0xFFF), 500B used for the interperter
* 1 16-Bit Memory (I) for storing the memory address (**However only 12 are usually used**)
* An 16 element long array of 16-bit values to hold return pointers (Included in that 500B used by the interperter)
* 15 8-Bit General Use Registers (V0-VE)
* 1 8-Bit Flag Register (VF)
* 2 8-Bit Registers used for delay and sound timers
* A 16-Key Keyboard (0-F)
* A 64x32 Pixel Display


# Panel
A "debugging panel" is being implemented to show the current state of the computers, when finished it will look something like this:

```
Registers:
|-0--|-1--|-2--|-3--|-4--|-5--|-6--|-7--|-8--|-9--|-A--|-B--|-C--|-D--|-E--|-F--|
| 01 | 0A | A4 | B4 | 6E | FF | FF | 00 | 3A | 33 | 23 | 9B | 4E | AA | B4 | 69 |
|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|

Stack          I: [ 0x220 ]
|-------|      LOG:
| 0x20A |      [ 0x21E ] 7561:  OR  V5, V6;
| 0x21B |      [ 0x21C ] 8555:  SUB V5, V5;
| 0x2B2 |      [ 0x21A ] F51E:  ADD I,  V5;
|-------|      [ 0x218 ] 8104:  ADD V1, V0;
```



This Project is initially based off a [CHIP-8 emulator](https://github.com/Elizabethtown-College-CS-Club/chip8-emulator) started by the [Elizabethtown College Computer Science Club](https://github.com/Elizabethtown-College-CS-Club)
