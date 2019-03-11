class Chocolate
{
	public:
		Chocolate(const char gameFile[]);
		Chocolate();
		void reset();
		void tick();
		bool getPixel(int, int);
		void setKey(int, bool);
		bool getKey(int);
		bool isBuzzer();

		//Opcodes
		void _00E0();
		void _00EE();
		void _1NNN(unsigned short);
		void _2NNN(unsigned short);
		void _3XNN(unsigned short);
		void _4XNN(unsigned short);
		void _5XY0(unsigned char);
		void _6XNN(unsigned short);
		void _7XNN(unsigned short);
		void _8XY0(unsigned char);
		void _8XY1(unsigned char);
		void _8XY2(unsigned char);
		void _8XY3(unsigned char);
		void _8XY4(unsigned char);
		void _8XY5(unsigned char);
		void _8XY6(unsigned char);
		void _8XY7(unsigned char);
		void _8XYE(unsigned char);
		void _9XY0(unsigned char);
		void _ANNN(unsigned short);
		void _BNNN(unsigned short);
		void _CXNN(unsigned short);
		void _DXYN(unsigned short);
		void _EX9E(unsigned char);
		void _EXA1(unsigned char);
		void _FX07(unsigned char);
		void _FX0A(unsigned char);
		void _FX15(unsigned char);
		void _FX18(unsigned char);
		void _FX1E(unsigned char);
		void _FX29(unsigned char);
		void _FX33(unsigned char);
		void _FX55(unsigned char);
		void _FX65(unsigned char);


	private:
		unsigned char memory[4096];
		unsigned char registers[16];
		unsigned short stack[16];
		int stackPointer;
		unsigned short address; // I
		unsigned short programCounter;

		unsigned char delayTimer;
		unsigned char soundTimer;

		// true following a tick where the sound timer expires
		bool buzzer;

		bool keyStates[16]; // true for pressed, false for unpressed

		bool pixels[64][32];
};
