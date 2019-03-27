#include "../include/Chocolate.hpp"
#include <fmt/core.h>

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

	logstmt = "";

}

void Chocolate::tick()
{
	//Log message
	

	//Put in the program counter
	logstmt += fmt::format("[{:X}]: ", programCounter);

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
		}break;
		// Only one opcode here 1NNN.
		case 0x1000:{
			_1NNN(opcode);
		}break;
		//Only one opcode here as well
		case 0x2000:{
			_2NNN(opcode);
		}break;
		case 0x3000:{
			_3XNN(opcode);	
		}break;
		case 0x4000:{
			_4XNN(opcode);
		}break;
		case 0x5000:{
			_5XY0(opcode);
		}break;
		case 0x6000:{
			_6XNN(opcode);
		}break;
		default:{
			logstmt += "Unknown Opcode Read";
			programCounter += 2;
		}
	}
	logstmt += "\n"; //Give it an endline
	logger.store(logstmt);
	logstmt = ("");
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
	logstmt += "(00E0 : 00E0) ";

	for(int i = 0; i < 64; i++){
		for(int j = 0; j < 64; j++){
			pixels[i][j] = false;
		}
	}
	//TODO: More verbose logging, build a useful string
	
	logstmt +=  "Screen Cleared";
	programCounter += 2;
}


/** 00E0
 * @brief Returns from a subroutine
 * 
 */
void Chocolate::_00EE(){
	logstmt += "(00EE : 00EE) ";

	if(stackPointer != 0){
		stackPointer--;
		programCounter = stack[stackPointer];
		programCounter += 2;
		//SH = Stack Height,
		logstmt += fmt::format("Returning from sub. SH[{}]",stackPointer);
	}
	else{
		logger.store("ERROR: Stack Underflow");
		exit(1);
	}
}

/** 1NNN
 * @brief Jumps to a memory address
 * 
 * @param opcode NNN: Memory Address to jump to 
 * 
 */
void Chocolate::_1NNN(unsigned short opcode){
	logstmt += fmt::format("(1NNN : {:X}) ",opcode & 0xFFFF);
	/*
	* By using the & function we are able to get parts of the opcode that we want
	* However if they are not in the last few digits they must be bit shifted
	*/
	unsigned short address = (opcode & 0x0FFF);
	//logstmt << "Jumping to: " << std::hex << address << std::dec;
	programCounter = address;
}

/** 2NNN
 * @brief Calls a subroutine at addr
 * 
 * @param opcode NNN: Memory address where subroutine is
 */
void Chocolate::_2NNN(unsigned short opcode){
	logstmt += fmt::format("(2NNN : {:X}) ",opcode & 0xFFFF);

	stack[stackPointer] = programCounter; //Put the current location on the stack
	unsigned short address = (opcode & 0x0FFF);
	
	//Log
	logstmt += fmt::format("Storing {:X} at Stack[{}]. Jumping to {:X}",programCounter,stackPointer,address);
	stackPointer++;//Increase the stack pointer
	programCounter = address; // Jump to new location
}

/** 3XNN
 * @brief Skips the next instruction if the the Vx equals NN
 * 
 * @param opcode Vx: The register to check
 *               NN: The value to check against
 */
void Chocolate::_3XNN(unsigned short opcode){
	logstmt += fmt::format("(3XNN : {:X}) ",opcode & 0xFFFF);

	int index = (opcode & 0x0F00) >> 8;
	int regNum = registers[index];
	int val = opcode & 0x00FF;

	logstmt += fmt::format("V[{}] =  {}({:X}) ",index,regNum,regNum);


	if(regNum == val){
		logstmt += fmt::format("== {}({:X}) Skipping Next Instruction",val,val);
		programCounter += 4;
	}
	else{
		logstmt += fmt::format("!= {}({:X}) Continuing",val,val);
		programCounter += 2;
	}

}

/** 4XNN
 * @brief Skips next instruction is Vx != NN (Opposite of 3XNN)
 * 
 * @param opcode Vx: The register to check
 *               NN: The value to check against
 */
void Chocolate::_4XNN(unsigned short opcode){
	logstmt += fmt::format("(4XNN : {:X}) ",opcode & 0xFFFF);
	int index = (opcode & 0x0F00) >> 8;
	int val = opcode & 0x00FF;
	int regNum = registers[index];


	logstmt += fmt::format("V[{}] =  {}({:X}) ",index,regNum,regNum);


	if(regNum != val){
		logstmt += fmt::format("!= {}({:X}) Skipping Next Instruction",val,val);
		programCounter += 4;
	}
	else{
		logstmt += fmt::format("== {}({:X}) Continuing",val,val);
		programCounter += 2;
	}

}


/** 5XY0
 *  @brief Skips the next instruction if V(x) = V(y)
 *
 *  @param opcode X: V(x) the first register
 *  		  Y: V(y) the second register
 *
 */
void Chocolate::_5XY0(unsigned short opcode){
	int x = (opcode & 0x0F00) >> 8;
	int y = (opcode & 0x00F0) >> 4;

	if(registers[x] == registers[y]){
		programCounter += 4;
	}
	else{
		programCounter += 2;
	}	
}



/** 6XNN
 * @brief Sets V(x) to NN 
 * 
 * @param opcode x: V(x) register to change
 *               NN: The data to set the register too
 *
 */
void Chocolate::_6XNN(unsigned short opcode){
	int index = (opcode & 0x0F00) >> 8;
	int val = opcode & 0x00FF;
	
	registers[index] = val;

	programCounter += 2;
	
}

/** 7XNN
 * @brief Adds NN to V(x) 
 * 
 * @param opcode x: V(x) the register to add too
 *              NN: The amount to add
 */
void Chocolate::_7XNN(unsigned short opcode){
	int index = (opcode & 0x0F00) >> 8;
	unsigned char add = opcode & 0x00FF;
	registers[index] += add;
}

/** 8XY0
 * @brief Sets V(x) to V(y)
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY0(unsigned short opcode){
	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = registers[yIndex];

	registers[xIndex] = yVal;

}

/** 8XY1
 * @brief Bitwise OR, sets V(x) to V(x)|V(y)
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY1(unsigned short opcode){
	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = registers[yIndex];
	unsigned char xVal = registers[xIndex];

	unsigned char orVal = xVal|yVal;

	registers[xIndex] = orVal;
}

/** 8XY2
 * @brief Bitwise AND, sets V(x) to V(x)&V(y)
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY2(unsigned short opcode){
	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = registers[yIndex];
	unsigned char xVal = registers[xIndex];

	unsigned char andVal = xVal&yVal;

	registers[xIndex] = andVal;
}

/**
 * @brief Bitwise XOR, sets V(x) to V(x)^V(y)
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY3(unsigned short opcode){
	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = registers[yIndex];
	unsigned char xVal = registers[xIndex];

	unsigned char xorVal = xVal^yVal;

	registers[xIndex] = xorVal;

}

void Chocolate::_8XY4(unsigned short opcode){

}

void Chocolate::_8XY5(unsigned short opcode){

}

void Chocolate::_8XY6(unsigned short opcode){

}

void Chocolate::_8XY7(unsigned short opcode){

}

void Chocolate::_8XYE(unsigned short opcode){

}

void Chocolate::_9XY0(unsigned short opcode){

}

void Chocolate::_ANNN(unsigned short opcode){

}

void Chocolate::_BNNN(unsigned short opcode){

}

void Chocolate::_CXNN(unsigned short opcode){

}

void Chocolate::_DXYN(unsigned short opcode){

}

void Chocolate::_EX9E(unsigned short opcode){

}

void Chocolate::_EXA1(unsigned short opcode){

}

void Chocolate::_FX07(unsigned short opcode){

}

void Chocolate::_FX0A(unsigned short opcode){

}

void Chocolate::_FX15(unsigned short opcode){

}

void Chocolate::_FX18(unsigned short opcode){

}

void Chocolate::_FX1E(unsigned short opcode){

}

void Chocolate::_FX29(unsigned short opcode){

}

void Chocolate::_FX33(unsigned short opcode){

}

void Chocolate::_FX55(unsigned short opcode){

}

void Chocolate::_FX65(unsigned short opcode){

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
