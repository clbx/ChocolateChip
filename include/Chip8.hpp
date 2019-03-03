class Chip8
{
	public:
		Chip8(const char gameFile[]);
		void reset();
		void tick();
		bool getPixel(int, int);
		void setKey(int, bool);
		bool getKey(int);
		bool isBuzzer();

		//Utility

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
