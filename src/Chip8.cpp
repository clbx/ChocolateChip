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
	getchar();

	printf("[%03X] ", programCounter);

	unsigned short opcode = memory[programCounter] << 8;
	opcode |= memory[programCounter + 1];

	// This offset (x2 bytes) will be applied to the program counter after the opcode is processed
	int pcDelta = 1;

	switch (opcode >> 12) {
		case 0x0: {
			if ((opcode & 0x000F) == 0x0) { // 00E0
				for (int i=0; i<64; i++)
					for (int j=0; j<32; j++)
						pixels[i][j] = false;
				printf("%04X (00E0): clearing the screen\n", opcode);
			} else { // 00EE
				stackPointer--; // make sure this is != 0
				programCounter = stack[stackPointer];

				printf("%04X (00EE): returning to address %03X\n", opcode, programCounter);
			}
		} break;
		case 0x1: { // 1NNN: Jump to address
			programCounter = opcode & 0x0FFF;
			pcDelta = 0;

			printf("%04X (1NNN): jumping to address %03X\n", opcode, programCounter);
		} break;
		case 0x2: { // 2NNN
			stack[stackPointer] = programCounter;
			stackPointer++;

			programCounter = opcode & 0x0FFF;
			pcDelta = 0;

			printf("%04X (2NNN): subroutine call to address %03X\n", opcode, programCounter);
		} break;
		case 0x3: { // 3XNN
			int reg = (opcode & 0x0F00) >> 8;
			unsigned char number = opcode & 0x00FF;
			if (registers[reg] == number)
				 pcDelta = 2;

			printf("%04X (3XNN): Comparison \"==\"\n", opcode);
		} break;
		case 0x4: { // 4XNN
			int reg = (opcode & 0x0F00) >> 8;
			unsigned char number = opcode & 0x00FF;
			if (registers[reg] != number)
				pcDelta = 2;

			printf("%04X (4XNN): Comparison \"!=\"\n", opcode);
		} break;
		case 0x5: { // 5XY0
			int reg1 = (opcode & 0x0F00) >> 8;
			int reg2 = (opcode & 0x00F0) >> 4;
			if (registers[reg1] == registers[reg2])
				pcDelta = 2;

			printf("%04X (5XY0): Comparison\n", opcode);
		} break;
		case 0x6: { // 6XNN
			int reg = (opcode & 0x0F00) >> 8;
			registers[reg] = opcode & 0x00FF;

			printf("%04X (6XNN): Placed value %d in register %X\n", opcode, opcode & 0xFF, reg);
		} break;
		case 0x7: { // 7XNN
			int reg = (opcode & 0x0F00) >> 8;
			registers[reg] += opcode & 0x00FF;

			printf("%04X (7XNN): Added value %d to register %X\n", opcode, opcode & 0xFF, reg);
		} break;
		case 0x8: {
			int regX = (opcode & 0x0F00) >> 8;
			int regY = (opcode & 0x00F0) >> 4;
			int value;

			switch(opcode & 0x000F)
			{
				case 0x0:
					registers[regX] = registers[regY];
					printf("%04X (8XY0): V[%X] = V[%X]\n", opcode, regX, regY);
					break;
				case 0x1:
					registers[regX] |= registers[regY];
					registers[0xF] = 0;
					printf("%04X (8XY1): V[%X] |= V[%X]\n", opcode, regX, regY);
					break;
				case 0x2:
					registers[regX] &= registers[regY];
					registers[0xF] = 0;
					printf("%04X (8XY2): V[%X] &= V[%X]\n", opcode, regX, regY);
					break;
				case 0x3:
					registers[regX] ^= registers[regY];
					registers[0xF] = 0;
					printf("%04X (8XY3): V[%X] ^= V[%X]\n", opcode, regX, regY);
					break;
				case 0x4:
					value = (int)registers[regX] + (int)registers[regY];
					if (value != (unsigned char)value)
						registers[0xF] = 1; // Overflow occurs
					else
						registers[0xF] = 0;
					registers[regX] = value;
					printf("%04X (8XY4): V[%X] += V[%X]\n", opcode, regX, regY);
					break;
				case 0x5:
					if (registers[regY] > registers[regX])
						registers[0xF] = 0; // Borrow occurs
					else
						registers[0xF] = 1;
					registers[regX] -= registers[regY];
					printf("%04X (8XY5): V[%X] -= V[%X]\n", opcode, regX, regY);
					break;
				case 0x6:
					value = registers[regX] & 1;
					registers[regX] >>= 1;
					registers[0xF] = value;
					printf("%04X (8XY6): V[%X] >>= 1\n", opcode, regX);
					break;
				case 0x7:
					if (registers[regX] > registers[regY])
						registers[0xF] = 0; // Borrow occurs
					else
						registers[0xF] = 1;
					registers[regX] = registers[regY] - registers[regX];
					printf("%04X (8XY7): V[%X] = V[%X] - V[%X]\n", opcode, regX, regX, regY);
					break;
				case 0xE:
					value = (registers[regX] >> 7) & 1;
					registers[regX] <<= 1;
					registers[0xF] = value;
					printf("%04X (8XYE): V[%X] <<= 1\n", opcode, regX);
					break;
				default:
					break;
			}
		} break;
		case 0xA: { // ANNN
			address = opcode & 0x0FFF;

			printf("%04X (ANNN): Setting memory address to %04X \n", opcode, opcode&0x0FFF);
		} break;
		case 0xB: { // BNNN
			unsigned short nnn = opcode & 0x0FFF;
			programCounter = registers[0] + nnn;
			pcDelta = 0;

			printf("%04X (BNNN): \n", opcode);
		} break;
		case 0xC: { // CXNN
			int random = rand();
			int x = (opcode & 0x0F00) >> 16;
			int n = opcode & 0x00FF;
			registers[x] = random & n;

			printf("%04X (CXNN): Placing random value masked with %02X in register %X\n", opcode, n, x);
		} break;
		case 0xD: { // DXYN
			int x = (opcode & 0x0F00) >> 8;
			int y = (opcode & 0x00F0) >> 4;
			int xPos = registers[x];
			int yPos = registers[y];
			int n = opcode & 0x000F;
			int memoryPos = address;
			bool changed = false;

			for(int row = yPos; row < yPos + n; row++) {
				int column = xPos;
				for(int j = 7; j >= 0; j--) {
					bool set = (memory[memoryPos] & (1 << j)) > 0;
					if(pixels[column][row] && !set)
						changed = true;
					pixels[column][row] = set;
			  		column++;
				}
				memoryPos++;
			}
			registers[15] = changed ? 1 : 0;

			printf("%04X (DXYN): drawing sprite\n", opcode);
		} break;
		case 0xF: {
			switch(opcode & 0x00FF){
				int registerPos;
				case 0x07:
					//Vx = delay timer  Vx = registers[x]
					//VF[15] is a intruction flag. Show a warning if this is used here.
					registerPos = (opcode & 0x0F00) >> 8;
					if(registerPos == 15){
						printf("warn: Register V[F] was assigned manually.\n");
					}
					registers[registerPos] = delayTimer;
					printf("%04X (FX07): V[%X] = Delay Timer: %X\n",opcode, registerPos, delayTimer);
				break;
				case 0x0A:
					bool keyCheck = false;
						printf("Stuck?");
						for(int i = 0; i < 16; i++){
							if(getKey(i)){
								printf("no!");
								registers[(opcode & 0x0F00) >> 8] = i;
								keyCheck = true;
							}
						}

						if(!keyCheck){
							pcDelta = 0;
						}
				break;
			}break;

		}
		default: {
			printf("Unsupported opcode %04X, skipping!\n", opcode);
		}break;
	}

	printf("%d",pcDelta);

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
