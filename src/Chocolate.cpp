#include "Chocolate.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>

Chocolate::Chocolate(const char gameFile[])
{
	reset();

	FILE* file = fopen(gameFile, "r");

	fread(memory + 0x200, 1, sizeof(memory) - 0x200, file);

	fclose(file);

	//for (int i = 0; i < sizeof(memory); i += 2)
	//	printf("(%d) %04X ", i, memory[i] << 8 | memory[i+1]);
}

Chocolate::Chocolate() //For Testing Purposes Only
{
	reset();
}

void Chocolate::reset()
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

void Chocolate::tick()
{

	printf("[%03X] ", programCounter);

	unsigned short opcode = memory[programCounter] << 8;
	opcode |= memory[programCounter + 1];

	switch (opcode >> 12) {

	}

}


/******************
OPCODE FUNCTIONS
Opcodes
*******************/

/** 00E0
 * @brief Clears the screen
 * 
 * 
 */
void Chocolate::_00E0(){
	for(int i = 0; i < 64; i++){
		for(int j = 0; j < 64; j++){
			pixels[i][j] = false;
		}
	}
}


/**
 * @brief Returns from a subroutine
 * 
 */
void Chocolate::_00EE(){
	if(stackPointer != 0){
		programCounter = stack[stackPointer];
		programCounter--;
	}
	else{
		fprintf(stderr,"ERROR: Stack Underflow");
	}

	
}

void Chocolate::_1NNN(unsigned short opcode){

}


void Chocolate::_2NNN(unsigned short opcode){

}

void Chocolate::_3XNN(unsigned short opcode){

}

void Chocolate::_4XNN(unsigned short opcode){

}


void Chocolate::_5XY0(unsigned char opcode){

}

void Chocolate::_6XNN(unsigned short opcode){

}

void Chocolate::_7XNN(unsigned short opcode){

}

void Chocolate::_8XY0(unsigned char opcode){

}

void Chocolate::_8XY1(unsigned char opcode){

}

void Chocolate::_8XY2(unsigned char opcode){

}

void Chocolate::_8XY3(unsigned char opcode){

}

void Chocolate::_8XY3(unsigned char opcode){

}

void Chocolate::_8XY4(unsigned char opcode){

}

void Chocolate::_8XY5(unsigned char opcode){

}

void Chocolate::_8XY6(unsigned char opcode){

}

void Chocolate::_8XY7(unsigned char opcode){

}

void Chocolate::_8XYE(unsigned char opcode){

}

void Chocolate::_9XY0(unsigned char opcode){

}

void Chocolate::_ANNN(unsigned short opcode){

}

void Chocolate::_BNNN(unsigned short opcode){

}

void Chocolate::_CXNN(unsigned short opcode){

}

void Chocolate::_DXYN(unsigned short opcode){

}

void Chocolate::_EX9E(unsigned char opcode){

}

void Chocolate::_EXA1(unsigned char opcode){

}

void Chocolate::_FX07(unsigned char opcode){

}

void Chocolate::_FX0A(unsigned char opcode){

}

void Chocolate::_FX15(unsigned char opcode){

}

void Chocolate::_FX18(unsigned char opcode){

}

void Chocolate::_FX1E(unsigned char opcode){

}

void Chocolate::_FX29(unsigned char opcode){

}

void Chocolate::_FX33(unsigned char opcode){

}

void Chocolate::_FX55(unsigned char opcode){

}

void Chocolate::_FX65(unsigned char opcode){

}








/***********************************
AUXILLARY FUNCTIONS
Functions that are commonly used 
************************************/


bool Chocolate::getPixel(int x, int y)
{
	return pixels[x][y];
}

void Chocolate::setKey(int key, bool isPressed)
{
	keyStates[key] = isPressed;
}

bool Chocolate::getKey(int key){
	if(keyStates[key] != 0)
		return 1;
	return 0;
}

bool Chocolate::isBuzzer()
{
	return buzzer;
}
