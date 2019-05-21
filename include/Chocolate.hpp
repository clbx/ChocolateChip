#include <stdint.h>
#include <iostream>
#include <random>


class Chocolate{

    private:
        uint8_t memory[4068];
        uint16_t I;
        
        uint16_t stack[16];
        uint16_t sp;

        uint8_t V[16];
        uint8_t delay;
        uint8_t sound;

        uint16_t pc;
        uint16_t pcDelta;
        uint16_t op;

        uint16_t NNN;
        uint8_t NN;
        uint8_t N;
        uint8_t X;
        uint8_t Y;


        uint8_t fontset[80] =
        {
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

        void _00E0();
        void _00EE();
        void _1NNN();
        void _2NNN();
        void _3XNN();
        void _4XNN();
        void _5XY0();
        void _6XNN();
        void _7XNN();
        void _8XY0();
        void _8XY1();
        void _8XY2();
        void _8XY3();
        void _8XY4();
        void _8XY5();
        void _8XY6();
        void _8XY7();
        void _8XYE();
        void _9XY0();
        void _ANNN();
        void _BNNN();
        void _CXNN();
        void _DXYN();
        void _EX9E();
        void _EXA1();
        void _FX07();
        void _FX0A();
        void _FX15();
        void _FX18();
        void _FX1E();
        void _FX29();
        void _FX33();
        void _FX55();
        void _FX65();

        void push(uint16_t);
        uint16_t pop();

    public:
        uint8_t graphics[2048];
        uint8_t keymap[16];
        bool draw;
        bool buzzer;

        Chocolate();

        void tick();
        void reset();
        bool load(const char*);
};

