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

    if(delay > 0)
        delay--;
    if(sound > 0)
        sound--;


    printf("%04X ",op);

    printf("NNN:%03X NN:%02X N:%1X X:%1X Y:%1X   I:%d (%03X)\n",NNN,NN,N,X,Y,I,I);

    for(int i = 0; i < 16; i++){
        printf("%d:%d ",i,V[i]);
    }
    printf("\n");

    

    pc += 2 * pcDelta;
}


/**
 * @brief Clears the screen
 * 
 */
void Chocolate::_00E0(){
    memset(graphics,0,2048);
    draw = true;
}

/**
 * @brief Return from a subroutine
 * 
 */
void Chocolate::_00EE(){
    pc = pop();             
    pcDelta = 2;               
}

/**
 * @brief Jumps to NNN
 * 
 */
void Chocolate::_1NNN(){
    pc = NNN;
    pcDelta = 0;
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

/*
void Chocolate::_3XNN(){
    pcDelta = (V[X] == NN) ? 2:1;
}*/

void Chocolate::_3XNN(){
    pcDelta = (V[X] == NN) ? 2:1;
}


/**
 * @brief Skips the next instructio if V[X] != NN
 * 
 */
void Chocolate::_4XNN(){
    pcDelta = (V[X] != NN) ? 2:1;
}


/**
 * @brief Skips the next instruction if V[X] == V[Y]
 * 
 */
void Chocolate::_5XY0(){
    pcDelta = (V[X] == V[Y]) ? 2:1;
}

/**
 * @brief Sets V[X] to NN
 * 
 */
void Chocolate::_6XNN(){
    V[X] = NN;
}

/**
 * @brief Adds NN to V[X] (Ignores Carry Flag)
 * 
 */
void Chocolate::_7XNN(){
    V[X] += NN;
}

/**
 * @brief Sets V[X] to V[Y]
 * 
 */
void Chocolate::_8XY0(){
    V[X] = V[Y];
}

/**
 * @brief Sets V[X] to V[X] | V[Y]
 * 
 */
void Chocolate::_8XY1(){
    V[X] = V[X] | V[Y];
}

/**
 * @brief Sets V[X] to V[X] & V[Y] 
 * 
 */
void Chocolate::_8XY2(){
    V[X] = V[X] & V[Y];    
}

/**
 * @brief Set V[X] to V[X] ^ V[Y]
 * 
 */
void Chocolate::_8XY3(){
    V[X] = V[X] ^ V[Y];
}

/**
 * @brief Adds V[Y] to V[X]. V[F] is set to 1 when there is a carry and 0 when there isn't
 * 
 */
void Chocolate::_8XY4(){
    V[0xF] = ((V[X] + V[Y]) > 0xFFFF) ? 1:0;
    V[X] += V[Y];
}

/**
 * @brief V[Y] is subtraced from V[X]. V[F] is set to 0 when there is a borrow and 1 where there isnt
 * 
 */
void Chocolate::_8XY5(){
    (V[Y] > V[X]) ? V[0xF]=0:V[0xF]=1;
    V[X] -= V[Y];
}

/**
 * @brief Stores the least signifigant bit of V[X] in V[F] and then shifts V[X] to the right by 1
 * 
 */
void Chocolate::_8XY6(){
    V[0xF] = V[X] & 0x0001; //Get least signifigant bit
    V[X] >>= 1;                    
}

/**
 * @brief Sets V[X] to V[Y] - V[X]. V[F] is set to 0 when there is a borrow, 1 when there isn't
 * 
 */
void Chocolate::_8XY7(){
    (V[Y] < V[X]) ? V[0xF]=0:V[0xF]=1;
    V[X] = V[Y] - V[X];
}

/**
 * @brief Stores the most signifigant bit of V[X] in V[F] then shifts V[F] to the left by 1
 * 
 */
void Chocolate::_8XYE(){
    V[0xF] = V[X] & 0x80; //Get most signifigant bit
    V[X] <<= 1;
}

/**
 * @brief Skips the next instruction if V[X] != V[Y]
 * 
 */
void Chocolate::_9XY0(){
    (V[X] != V[Y]) ? pcDelta =2:pcDelta=1;
}

/**
 * @brief Sets I to the address NNN
 * 
 */
void Chocolate::_ANNN(){
    I = NNN;
}

/**
 * @brief Jups to the address NNN plus V[0]
 * 
 */
void Chocolate::_BNNN(){
    I = NNN + V[0];
}

/**
 * @brief Sets V[X] to the result of a bitwise and operation on a random number (Typically 0 to 255) and NN
 * 
 */
void Chocolate::_CXNN(){
    V[X] = 1;
    //V[X] = (rand() % (0xFF +1)) & NN;
}

/**
 * @brief Draws a sprite at (V[X],V[Y]) that has a width of 8 pixels and a height of N pixels
 * Each row of 8 blices is read as a bit-coded from memory location I; I value doesn't change after 
 * execution. As described above V[F] is set to 1 if any pixels are flipped from the set to unset when the
 * sprite is drawn, and to 0 if that doesn't happen
 */
void Chocolate::_DXYN(){

    uint16_t pixel;
    
    V[0xF] = 0;

    for(int i = 0; i < N; i++){
        pixel = memory[I+i];
        for(int j = 0; j < 8; j++){
            if((pixel & (0x80 >> j)) != 0){
                if(graphics[(X + j + ((Y+ i) * 64)) ] == 1){
                    V[0xF] = 1;
                }
                graphics[X + j + ((Y+i) * 64)] ^= 1;
            }
        }
    }
    draw = true;
}


/**
 * @brief Skips the next instruction if the key stored in V[X] is pressed
 * 
 */
void Chocolate::_EX9E(){
    pcDelta = (keymap[V[X]] != 0) ? 2:1;    
}

/**
 * @brief Skips the next instruction if the key stored in V[X] isn't pressed
 * 
 */
void Chocolate::_EXA1(){
    pcDelta = (keymap[V[X]] != 0) ? 2:1;
}

/**
 * @brief Sets V[X] to the value of the delay timer
 * 
 */
void Chocolate::_FX07(){
    V[X] = delay;
}

/**
 * @brief A key press is awaited, and then stored in V[X]
 * 
 */
void Chocolate::_FX0A(){
    bool keypress = false;

    for(int i = 0; i < 16; i++){
        if(keymap[i] != 0){
            V[X] = i;
            keypress = true;
        }
    }
    if(!keypress){
        pcDelta = 0;
    }
}

/**
 * @brief Sets the delay timer to V[X]
 * 
 */
void Chocolate::_FX15(){
    delay = V[X];    
}

/**
 * @brief Sets the sound timer to V[X]
 * 
 */
void Chocolate::_FX18(){
    sound = V[X];
}

/**
 * @brief Adds V[X] to I
 * 
 */
void Chocolate::_FX1E(){
    I += V[X];
}

/**
 * @brief Sets I to the location of the sprice to the character in V[X] (0-F)
 * 
 */
void Chocolate::_FX29(){
    I = V[X] * 0x5;
}

/**
 * @brief Stores the BCD respresentation of V[X]. 100's digit at I, 10's at I+1, I's at I+2
 */
void Chocolate::_FX33(){
        memory[I] = V[X] /100;
        memory[I+1] = (V[X] /10)%10;
        memory[I+2] = V[X] % 10;
}

/**
 * @brief Stores V[0] to V[X] in memory starting at address I. The offset from I is increased
 * by 1 for each value written but I itself is left unmodified
 */
void Chocolate::_FX55(){  
    for(int i = 0; i <= V[X]; i++){
        memory[I + i] = V[i];
    }
}

/**
 * @brief Fills V[0] to V[X] with falues from memory starting at address I. The offset from I
 * is increased by 1 for each value written, but I itself is left unmodified 
 * 
 */
void Chocolate::_FX65(){
    for(int i = 0; i <= V[X]; i++){
        V[i] = memory[I+i];
    }
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