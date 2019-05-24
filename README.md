# ChocolateChip
🍪 Round 2 With the CHIP-8 Emulator

All roms I tested work

Things left I want to do:

- [ ] Make opcode selection more efficient
- [ ] Finish up commands
- [ ] Try and reduce flickering
- [ ] Make a GUI

# Some very useful resources:

[Chip 8's Wikipedia Page](https://en.wikipedia.org/wiki/CHIP-8)

[Cowgod's Chip 8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)


# Improved Stuff in Rewrite;

* **SDL instead of SFML** I've found SDL to be much easier to use, plus it's much more widespread.
* **Much shorter and concise operations** All the usual operands are grabbed at the beginning, makes code _much_ shorter (most of the opcodes are one-liners), consistent, and less prone to error.

* **Fixed a bunch of bugs** The old version had some rampant bug issues that were hard to track down due to how it was written
