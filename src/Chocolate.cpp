#include "../include/Chocolate.hpp"
#include <fmt/core.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>


unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6    
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

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
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));
	memset(keyStates, 0, sizeof(keyStates));

	stackPointer = 0;
	I = 200;
	programCounter = 0x200;

	delayTimer = 0;
	soundTimer = 0;

	buzzer = false;

	drawFlag = false;

	memset(pixels, 0, 2048);

	//Load font into memory


	for(int i = 0; i < 80; i++){
		memory[i] = chip8_fontset[i];
	}

	logstmt = "";

}

void Chocolate::tick()
{
	//Log message
	
	//Put in the program counter
	logstmt += fmt::format("[{:X}]: ", programCounter);

	unsigned short opcode = memory[programCounter] << 8;
	opcode |= memory[programCounter + 1];

	logstmt += fmt::format("{:X}",opcode);
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
		case 0x7000:{
			_7XNN(opcode);
		}break;
		case 0x8000:{
			switch(opcode & 0x000F){
				case 0x0000:{
					_8XY0(opcode);
				}break;
				case 0x0001:{
					_8XY1(opcode);
				}break;
				case 0x0002:{
					_8XY2(opcode);
				}break;
				case 0x0003:{
					_8XY3(opcode);
				}break;
				case 0x0004:{
					_8XY4(opcode);
				}break;
				case 0x0005:{
					_8XY5(opcode);
				}break;
				case 0x0006:{
					_8XY6(opcode);
				}break;
				case 0x0007:{
					_8XY7(opcode);
				}break;
				case 0x000E:{
					_8XYE(opcode);
				}break;
			}
		}break;
		case 0x9000:{
			_9XY0(opcode);
		}break;
		case 0xA000:{
			_ANNN(opcode);
		}break;
		case 0xB000:{
			_BNNN(opcode);
		}break;
		case 0xC000:{
			_CXNN(opcode);
		}break;
		case 0xD000:{
			_DXYN(opcode);
		}break;
		case 0xE000:{
			switch(opcode & 0x000F){
				case 0x000E:{
					_EX9E(opcode);
				}break;
				case 0x0001:{
					_EXA1(opcode);
				}break;
			}
		}break;
		case 0xF000:{
			switch(opcode & 0x00FF){
				case 0x0007:{
					_FX07(opcode);
				}break;
				case 0x000A:{
					_FX0A(opcode);
				}break;
				case 0x0015:{
					_FX15(opcode);
				}break;
				case 0x001E:{
					_FX1E(opcode);
				}break;
				case 0x0055:{
					_FX55(opcode);
				}break;
				case 0x0065:{
					_FX65(opcode);
				}break;
			}
		}break;
		default:{
			logstmt += fmt::format("Unknown Opcode Read {:X}",opcode);
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

	for(int i = 0; i < 2048; i++){
		pixels[i] = 0;
	}
	
	logstmt += "Screen Cleared";

	drawFlag = true;
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
	unsigned short I = (opcode & 0x0FFF);
	logstmt += fmt::format("Jumping to address: {:X}",I);
	//logstmt << "Jumping to: " << std::hex << address << std::dec;
	programCounter = I;
}

/** 2NNN
 * @brief Calls a subroutine at addr
 * 
 * @param opcode NNN: Memory address where subroutine is
 */
void Chocolate::_2NNN(unsigned short opcode){
	logstmt += fmt::format("(2NNN : {:X}) ",opcode & 0xFFFF);

	stack[stackPointer] = programCounter; //Put the current location on the stack
	unsigned short I = (opcode & 0x0FFF);
	
	//Log
	logstmt += fmt::format("Storing {:X} at Stack[{}]. Jumping to {:X}",programCounter,stackPointer,I);
	stackPointer++;//Increase the stack pointer
	programCounter = I; // Jump to new location
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
	int regNum = V[index];
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
	int regNum = V[index];


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
	logstmt += fmt::format("(5XY0 : {:X}) ",opcode & 0xFFFF);

	int x = (opcode & 0x0F00) >> 8;
	int y = (opcode & 0x00F0) >> 4;

	unsigned char xVal = V[x];
	unsigned char yVal = V[y];


	if(xVal == yVal){
		logstmt += fmt::format("V[{}] = {:X} == V[{}] = {:X} Skipping Next Instruction",x,xVal,y,yVal);
		programCounter += 4;
	}
	else{
		logstmt += fmt::format("V[{}] = {:X} != V[{}] = {:X} Continuing",x,xVal,y,yVal);
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
	logstmt += fmt::format("(6XNN : {:X}) ",opcode & 0xFFFF);

	int index = (opcode & 0x0F00) >> 8;
	int val = opcode & 0x00FF;
	
	logstmt += fmt::format("V[{}] = {:X}",index,val);

	V[index] = val;

	programCounter += 2;
	
}

/** 7XNN
 * @brief Adds NN to V(x) 
 * 
 * @param opcode x: V(x) the register to add too
 *              NN: The amount to add
 */
void Chocolate::_7XNN(unsigned short opcode){
	logstmt += fmt::format("(7XNN : {:X}) ",opcode & 0xFFFF);

	int index = (opcode & 0x0F00) >> 8;
	unsigned char add = opcode & 0x00FF;
	logstmt += fmt::format("V[{}] += {}",index,add);
	V[index] += add;

	programCounter += 2;
}

/** 8XY0
 * @brief Sets V(x) to V(y)
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY0(unsigned short opcode){
	logstmt += fmt::format("(8XY0 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	logstmt += fmt::format("V[{}] = V[{}]",xIndex,yIndex);

	unsigned char yVal = V[yIndex];

	V[xIndex] = yVal;

	programCounter += 2;

}

/** 8XY1
 * @brief Bitwise OR, sets V(x) to V(x)|V(y)
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY1(unsigned short opcode){
	logstmt += fmt::format("(8XY1 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = V[yIndex];
	unsigned char xVal = V[xIndex];

	unsigned char orVal = xVal|yVal;

	V[xIndex] = orVal;

	programCounter += 2;
}

/** 8XY2
 * @brief Bitwise AND, sets V(x) to V(x)&V(y)
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY2(unsigned short opcode){
	logstmt += fmt::format("(8XY2 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = V[yIndex];
	unsigned char xVal = V[xIndex];

	unsigned char andVal = xVal&yVal;

	V[xIndex] = andVal;

	programCounter += 2;
}

/**
 * @brief Bitwise XOR, sets V(x) to V(x)^V(y)
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY3(unsigned short opcode){
	logstmt += fmt::format("(8XY3 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = V[yIndex];
	unsigned char xVal = V[xIndex];

	unsigned char xorVal = xVal^yVal;

	V[xIndex] = xorVal;

	programCounter += 2;

}

/** 8XY4
 * @brief V[x] += V[y]
 * 
 * @param opcode x: the first register 
 *               y: the second register
 */       

void Chocolate::_8XY4(unsigned short opcode){
	logstmt += fmt::format("(8XY4 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = V[yIndex];
	unsigned char xVal = V[xIndex];

	V[xIndex] = yVal + xVal;

	if(xVal + yVal > 0xFF){
		V[0xF] = 1;
	}
	else{
		V[0xF] = 0;
	}
	
	programCounter += 2;

}

/**
 * @brief V[x] -= V[y]
 * 
 * @param opcode x: the first register
 *               y: the second register
 */
void Chocolate::_8XY5(unsigned short opcode){
	logstmt += fmt::format("(8XY5 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = V[yIndex];
	unsigned char xVal = V[xIndex];

	V[xIndex] = yVal - xVal;

	if(xVal - yVal < 0xFF){
		V[0xF] = 1;
	}
	else{
		V[0xF] = 0;
	}
	
	programCounter += 2;

}

/**
 * @brief 
 * 
 * @param opcode 
 */
void Chocolate::_8XY6(unsigned short opcode){
	logstmt += fmt::format("(8XY6 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	V[0xF] = V[xIndex] & 0x1;
	V[yIndex] >>= 1;

	programCounter += 2;
}

/**
 * @brief 
 * 
 * @param opcode 
 */
void Chocolate::_8XY7(unsigned short opcode){
	logstmt += fmt::format("(8XY7 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = V[yIndex];
	unsigned char xVal = V[xIndex];

	V[xIndex] = yVal - xVal;

	if(xVal > yVal){
		V[0xF] = 0;
	}
	else{
		V[0xF] = 1;
	}

	programCounter += 2;
}

void Chocolate::_8XYE(unsigned short opcode){
	logstmt += fmt::format("(8XYE : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;

	V[0xF] = V[xIndex] >> 7;
	V[xIndex] <<= 1;

	programCounter += 2;

}

void Chocolate::_9XY0(unsigned short opcode){
	logstmt += fmt::format("(9XY0 : {:X}) ",opcode & 0xFFFF);

	int xIndex = (opcode & 0x0F00) >> 8;
	int yIndex = (opcode & 0x00F0) >> 4;

	unsigned char yVal = V[yIndex];
	unsigned char xVal = V[xIndex];

	if(xVal != yVal){
		programCounter += 4;
	}
	else
	{
		programCounter += 2;
	}
	
}

void Chocolate::_ANNN(unsigned short opcode){
	logstmt += fmt::format("(ANNN : {:X}) ",opcode & 0xFFFF);

	unsigned short addr = opcode & 0x0FFF;

	logstmt += fmt::format("I = {:X}",addr);
	I = addr; 

	programCounter += 2;

}

void Chocolate::_BNNN(unsigned short opcode){
	logstmt += fmt::format("(BXNN : {:X}) ",opcode & 0xFFFF);

	unsigned short addr = (opcode & 0x0FFF);
	unsigned char addVal = V[0];


	logstmt += fmt::format("I = I + V[0]  -   I = {:X} + {:X}",addr,addVal);
	I = addr + addVal;

}

void Chocolate::_CXNN(unsigned short opcode){
	logstmt += fmt::format("(CXNN : {:X}) ",opcode & 0xFFFF);

	int index = (opcode & 0x0F00) >> 8;
	unsigned char mask = (opcode & 0x00FF);
	unsigned char val = rand() & mask;
	logstmt += fmt::format("Setting V[{:X}] to random value {}, Mask {}",index,val,mask);

	V[index] = val; 

	programCounter += 2;
}

void Chocolate::_DXYN(unsigned short opcode){
	logstmt += fmt::format("(DXYN : {:X}) ",opcode & 0xFFFF);

	unsigned short x = V[(opcode & 0x0F00) >> 8];
    unsigned short y = V[(opcode & 0x00F0) >> 4];
    unsigned short height = opcode & 0x000F;
    unsigned short pixel;

	logstmt += fmt::format("Drawing sprite from I={}, At ({},{}) Height {}",I,x,y,height);

    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++){
        pixel = memory[I + yline];
        for(int xline = 0; xline < 8; xline++){
            if((pixel & (0x80 >> xline)) != 0){
                if(pixels[(x + xline + ((y + yline) * 64))] == 1){
                    V[0xF] = 1;
                }
                pixels[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }

	drawFlag = true;

    programCounter += 2;

}

void Chocolate::_EX9E(unsigned short opcode){
	logstmt += fmt::format("(EX9E : {:X}) ",opcode & 0xFFFF);

	unsigned char reg = (opcode & 0x0F00) >> 8;
	bool key = keyStates[reg];

	if(key){
		programCounter += 4;
	}
	else{
		programCounter += 2;
	}
}

void Chocolate::_EXA1(unsigned short opcode){
	logstmt += fmt::format("(EXA1 : {:X}) ",opcode & 0xFFFF);

	unsigned char reg = (opcode & 0x0F00) >> 8;
	bool key = keyStates[reg];

	if(!key){
		programCounter += 4;
	}
	else{
		programCounter += 2;
	}
}

void Chocolate::_FX07(unsigned short opcode){
	logstmt += fmt::format("(FX07 : {:X}) ",opcode & 0xFFFF);

	unsigned char reg = (opcode & 0x0F00) >> 8;
	
	V[reg] = delayTimer;

	programCounter += 2;
}

void Chocolate::_FX0A(unsigned short opcode){
	logstmt += fmt::format("(FX0A : {:X}) ",opcode & 0xFFFF);

	bool pressed = false;
	unsigned char reg = (opcode & 0x0F00) >> 8;


	for(int i = 0; i < 16; i++){
		if(keyStates[i]){
			logstmt += fmt::format("Got keypress");
			V[reg] = i;
			pressed = true;
		}
	}
	if(pressed){
		programCounter += 2;
	}
}

void Chocolate::_FX15(unsigned short opcode){
	logstmt += fmt::format("(FX0A : {:X}) ",opcode & 0xFFFF);

	unsigned char reg = (opcode & 0x0F00) >> 8;
	unsigned char val = V[reg];

	delayTimer = val;

	programCounter += 2;
}

void Chocolate::_FX18(unsigned short opcode){

}


void Chocolate::_FX1E(unsigned short opcode){
	logstmt += fmt::format("(FX1E : {:X}) ",opcode & 0xFFFF);

	unsigned char reg = (opcode & 0x0F00) >> 8;
	unsigned char val = V[reg];

	if((val + I) > 0xFFF){
		V[0xF] = 1;
	}
	else{
		V[0xF] = 0;
	}

	I += val;

	programCounter += 2;

}

void Chocolate::_FX29(unsigned short opcode){

}

void Chocolate::_FX33(unsigned short opcode){

}

void Chocolate::_FX55(unsigned short opcode){
	logstmt += fmt::format("(FX1E : {:X}) ",opcode & 0xFFFF);

	unsigned char limit = (opcode & 0x0F00) >> 8;

	for(int i = 0; i <= limit; i++){
		memory[I + i] = V[i];
	}

	I += limit + 1;

	programCounter += 2;

}

void Chocolate::_FX65(unsigned short opcode){
	logstmt += fmt::format("(FX65 : {:X}) ",opcode & 0xFFFF);

	unsigned char limit = (opcode & 0x0F00) >> 8;

	for(int i = 0; i <= limit; i++){
		V[i] = memory[I + i];
	}

	I += limit + 1;

	programCounter += 2;

}








/***********************************
AUXILLARY FUNCTIONS
Functions that are commonly used 
************************************/

int Chocolate::getAddress(){
	return I;
}

bool Chocolate::getPixel(int x){
	return pixels[x];
}

void Chocolate::setPixel(int x, int y, bool value){
	pixels[x + 64*y] = value;
}

void Chocolate::setKey(int key, bool isPressed){
	keyStates[key] = isPressed;
}

bool Chocolate::getKey(int key){
	if(keyStates[key] != 0)
		return 1;
	return 0;
}

bool Chocolate::isBuzzer(){
	return buzzer;
}
