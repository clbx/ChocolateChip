#include "Chocolate.hpp"

/**
 * @brief Construct a new Chocolate:: Chocolate object
 * 
 */
Chocolate::Chocolate(){}

/**
 * @brief resets the emulator
 * 
 */
void Chocolate::reset(){
	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));
	memset(keymap, 0, sizeof(keymap));

    sp = 0;
    I = 200;
    pc = 0x200;

    delay = 0;
    sound = 0;

    buzzer = false;
    draw = false;

    memset(graphics,0,2048);

    for(int i = 0; i < 80; i++){
        memory[i] = fontset[i];
    }

}

/**
 * @brief loads a ROM into memory
 * 
 * @param filename 
 * @return true file loaded successfully
 * @return false file loading failed
 */
bool Chocolate::load(const char* filename){

    reset();

    FILE* file = fopen(filename, "r");
    fread(memory + 0x200,1,sizeof(memory) - 0x200, file);
    fclose(file);

    //TODO: Come back and add validation
    return true;

}

/**
 * @brief ticks the cpu once
 * 
 */
void Chocolate::tick(){
    //Get Opcode
    op = memory[pc] << 8 | memory[pc +1 ];
  
    //Get operands
    NNN = (op & 0x0FFF);
    NN = (op & 0x00FF);
    N = (op & 0x000F);
    X = (op & 0x0F00) >> 8;
    Y = (op & 0x00F0) >> 4;

    printf("%04X\n",op);

    switch(op & 0xF000){
        case 0x0000:{
            switch(op & 0x000F){
                case 0x0000:{}break;
                case 0x000E:{}break;
            }
        }break;
        case 0x1000:{}break;
        case 0x2000:{}break;
        case 0x3000:{}break;
        case 0x4000:{}break;
        case 0x5000:{}break;
        case 0x6000:{}break;
        case 0x7000:{}break;
        case 0x8000:{
            switch(op & 0x000F){
                case 0x0000:{}break;
                case 0x0001:{}break;
                case 0x0002:{}break;
                case 0x0003:{}break;
                case 0x0004:{}break;
                case 0x0005:{}break;
                case 0x0006:{}break;
                case 0x0007:{}break;
                case 0x000E:{}break;
            }
        }break;
        case 0x9000:{}break;
        case 0xA000:{}break;
        case 0xB000:{}break;
        case 0xC000:{}break;
        case 0xD000:{}break;
        case 0xE000:{}break;
        case 0xF000:{
            switch(op & 0x0FF){
                case 0x0007:{}break;
                case 0x000A:{}break;
                case 0x0015:{}break;
                case 0x0018:{}break;
                case 0x0029:{}break;
                case 0x001E:{}break;
                case 0x0033:{}break;
                case 0x0055:{}break;
                case 0x0065:{}break;
            }
        }break;
        default:{printf("unknown opcode");}
    }

    pc += 2;


}

