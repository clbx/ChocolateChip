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

    //Reset Program Counter Delta
    pcDelta = 1;

    printf("%04X\n",op);

    switch(op & 0xF000){
        case 0x0000:{
            switch(op & 0x000F){
                case 0x0000:{_00E0();}break;
                case 0x000E:{_00EE();}break;
            }
        }break;
        case 0x1000:{_1NNN();}break;
        case 0x2000:{_2NNN();}break;
        case 0x3000:{_3XNN();}break;
        case 0x4000:{_4XNN();}break;
        case 0x5000:{_5XY0();}break;
        case 0x6000:{_6XNN();}break;
        case 0x7000:{_7XNN();}break;
        case 0x8000:{
            switch(op & 0x000F){
                case 0x0000:{_8XY0();}break;
                case 0x0001:{_8XY1();}break;
                case 0x0002:{_8XY2();}break;
                case 0x0003:{_8XY3();}break;
                case 0x0004:{_8XY4();}break;
                case 0x0005:{_8XY5();}break;
                case 0x0006:{_8XY6();}break;
                case 0x0007:{_8XY7();}break;
                case 0x000E:{_8XYE();}break;
            }
        }break;
        case 0x9000:{_9XY0();}break;
        case 0xA000:{_ANNN();}break;
        case 0xB000:{_BNNN();}break;
        case 0xC000:{_CXNN();}break;
        case 0xD000:{_DXYN();}break;
        case 0xE000:{
            switch(op & 0x000F){
                case 0x000E:{_EX9E();}break;
                case 0x0001:{_EXA1();}break;
            }
        }break;
        case 0xF000:{
            switch(op & 0x0FF){
                case 0x0007:{_FX07();}break;
                case 0x000A:{_FX0A();}break;
                case 0x0015:{_FX15();}break;
                case 0x0018:{_FX18();}break;
                case 0x0029:{_FX29();}break;
                case 0x001E:{_FX1E();}break;
                case 0x0033:{_FX33();}break;
                case 0x0055:{_FX55();}break;
                case 0x0065:{_FX65();}break;
            }
        }break;
        default:{printf("unknown opcode");}
    }

    pc += 2 * pcDelta;
}


/**
 * @brief Clears the screen
 * 
 */
void Chocolate::_00E0(){
    memset(graphics,0,2048);            //Set graphics memory to 0
    draw = true;                        //Tell SDL to render new screen
}

/**
 * @brief Return from a subroutine
 * 
 */
void Chocolate::_00EE(){
    pc = pop();                         //Pop from stack to get return location
    pcDelta = 2;                        //Go to instruction after
}

/**
 * @brief Jumps to NNN
 * 
 */
void Chocolate::_1NNN(){
    pc = NNN;
}

/**
 * @brief Calls subroutine at NNN
 * 
 */
void Chocolate::_2NNN(){
    push(pc);
    pc = NNN;
}

/**
 * @brief Skips the next instruction if V[X] = NN
 * 
 */
void Chocolate::_3XNN(){
    (V[X] == NN) ? pcDelta = 2:pcDelta=1;
}

void Chocolate::_4XNN(){
    (V[X] != NN) ? pcDelta = 2:pcDelta=1;
}
void Chocolate::_5XY0(){
    
}
void Chocolate::_6XNN(){

}
void Chocolate::_7XNN(){

}
void Chocolate::_8XY0(){

}
void Chocolate::_8XY1(){

}
void Chocolate::_8XY2(){

}
void Chocolate::_8XY3(){

}
void Chocolate::_8XY4(){

}
void Chocolate::_8XY5(){

}
void Chocolate::_8XY6(){

}
void Chocolate::_8XY7(){

}
void Chocolate::_8XYE(){

}
void Chocolate::_9XY0(){

}
void Chocolate::_ANNN(){

}
void Chocolate::_BNNN(){

}
void Chocolate::_CXNN(){

}
void Chocolate::_DXYN(){

}
void Chocolate::_EX9E(){

}
void Chocolate::_EXA1(){

}
void Chocolate::_FX07(){

}
void Chocolate::_FX0A(){

}
void Chocolate::_FX15(){

}
void Chocolate::_FX18(){

}
void Chocolate::_FX1E(){

}
void Chocolate::_FX29(){

}
void Chocolate::_FX33(){

}
void Chocolate::_FX55(){

}
void Chocolate::_FX65(){

}


/**
 * @brief pushes a value onto the stack
 * 
 * @param val 
 */
void Chocolate::push(uint16_t val){
    stack[sp++] = val;
}

/**
 * @brief pops the value on the top of the stack
 * 
 * @return uint16_t 
 */
uint16_t Chocolate::pop(){
    return stack[sp--];
}