#include "Chip8.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>

Chip8::Chip8(const char gameFile[])
{
	reset();

	FILE* file = fopen(gameFile, "r");

	fread(memory + 0x200, 1, sizeof(memory) - 0x200, file);

	fclose(file);

	//for (int i = 0; i < sizeof(memory); i += 2)
	//	printf("(%d) %04X ", i, memory[i] << 8 | memory[i+1]);
}

void Chip8::reset()
{
	memset(memory, 0, sizeof(memory));
	memset(registers, 0, sizeof(registers));
	memset(stack, 0, sizeof(stack));
	memset(keyStates, 0, sizeof(keyStates));

	stackPointer = 0;
	address = 0;
	programCounter = 0x200;

	delayTimer = 0;
	soundTimer = 0;

	buzzer = false;

	memset(pixels, 0, 64*32);

}

void Chip8::tick()
{

	printf("[%03X] ", programCounter);

	unsigned short opcode = memory[programCounter] << 8;
	opcode |= memory[programCounter + 1];

	// This offset (x2 bytes) will be applied to the program counter after the opcode is processed
	int pcDelta = 1;

	switch (opcode >> 12) {
	}
	programCounter += 2 * pcDelta;

}

bool Chip8::getPixel(int x, int y)
{
	return pixels[x][y];
}

void Chip8::setKey(int key, bool isPressed)
{
	keyStates[key] = isPressed;
}

bool Chip8::getKey(int key){
	if(keyStates[key] != 0)
		return 1;
	return 0;
}

bool Chip8::isBuzzer()
{
	return buzzer;
}
