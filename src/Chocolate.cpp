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

	Logger logger;

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

	/* Here we will check the opcode received and go through digit by
	* digit to reach the corrrect opcode
	*
	* First we check the first digit by &ing it with 0xF000,
	* After that we go to the next digit
	*
	* While we could check each individual opcode, this is faster and
	* better practice
	*/
	switch (opcode & 0xF000) {
		// All 0x0 Opcodes
		case 0x0000:{
			/* Since there are only 2 opcodes for the 0's (00E0, and 00EE)
			*  We'll only check the last digit since its the only difference
			*/
			switch( opcode & 0x000F){
				// 00E0
				case 0x0000:{
					_00E0();
				}break;
				// 00EE
				case 0x000E:{
					_00EE();
				}break;
				default:{}break;
			}
		}
		// Only one opcode here 1NNN.
		case 0x1000:{
			/*Here we also handle giving all the nessessary data to the function
			* 1NNN needs a memory address, which is stored in an unsigned short
			*
			* An unsigned short is use because it is 2 bytes (16 bits)
			* and a memory address is 1.5 bytes (12 bits), but you can't get half a byte
			*
			* It's unsigned to prevent any kind of werid stuff with signs
			*/
			unsigned short addr = (0x0FFF & opcode);
			_1NNN(addr);
		}
		//Only one opcode here as well
		case 0x2000:{
			unsigned short addr = (0x0FFF & opcode);
			_2NNN(addr);
		}
		case 0x3000:{

		}
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
	logger.store("(00E0) Clearing Screen");
	programCounter += 2;
	
}


/** 00E0
 * @brief Returns from a subroutine
 * 
 */
void Chocolate::_00EE(){
	if(stackPointer != 0){
		stackPointer--;
		programCounter = stack[stackPointer];
		programCounter += 2;
	}
	else{
		logger.store("ERROR: Stack Underflow");
		exit(1);
	}
}

void Chocolate::_1NNN(unsigned short addr){

}


void Chocolate::_2NNN(unsigned short addr){

}

void Chocolate::_3XNN(unsigned char reg, unsigned char val){

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
