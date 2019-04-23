#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fmt/core.h>
#include "Chocolate.hpp"

int convertKeycode(sf::Keyboard::Key);
int charKeycode(char key);

int main()
{
	// Load CHIP-8 buzzer sound


	char rom[64]; //= (char*)"rom/t";
	char inDebug = 'y';
	char inManual = 'y';
	bool debug = true;
	bool manual = true;

	
	printf("Enter ROM file path: ");
	scanf("%s",rom);
	printf("Manual Clock? (y/n) ");
	scanf(" %c", &inManual);
	printf("Debug? (y/n) ");
	scanf(" %c", &inDebug);

	if(inManual == 'n'){
		manual = false;
	}

	if(inDebug == 'n'){
		debug = false;
	}
	


	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("res/beep.wav")) {
		std::cerr << "Unable to load buzzer sound file, exiting" << std::endl;
		return -1;
	}
	sf::Sound beep;
	beep.setBuffer(buffer);
	//beep.play(); // Startup test beep (why not?)



	// Non-resizable window with a titlebar and a "close" button
	sf::RenderWindow window(sf::VideoMode(480, 240), "CHIP-8 Display", sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(60); // Force application to run near 60 FPS (CHIP-8 runs at 60Hz)

	// A 64x32 pixel "view," used to specify the part of window's internal pixel array that we actually want to display
	sf::View view(sf::FloatRect(0, 0, 64, 32));
	window.setView(view);

	// A white rectangle that takes up a single pixel, used to perform pixel-by-pixel drawing on our monochrome screen
	sf::RectangleShape pixel(sf::Vector2f(1, 1));
	pixel.setFillColor(sf::Color::White);

	bool keyState[16]; // Records whether each key is currently being pressed (true) or not (false)
	for (int i = 0; i < 16; i++) // Necessary to prevent array being filled with random values
		keyState[i] = false;

	// Create the CHIP-8 emulator
	Chocolate chip8(rom);

	while (window.isOpen())
	{
		// Process any new events that occurred
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			// Record when keys are pressed. Press events do not occur consistently while a key is held down, necessitating further state-tracking through the keyState array.
			} else if (event.type == sf::Event::KeyPressed) {
				int keycode = convertKeycode(event.key.code);
				if (keycode != -1)
					keyState[keycode] = true;
			// Record when keys are released
			} else if (event.type == sf::Event::KeyReleased) {
				int keycode = convertKeycode(event.key.code);
				if (keycode != -1)
					keyState[keycode] = false;
			}
		}

		// Alert the CHIP-8 to any keys currently held down
		for (int i = 0; i < 16; i++)
			chip8.setKey(i, keyState[i]);

		// Let the CHIP-8 process events and render graphics to its buffer
		
		
		if(manual){
			getchar();
		}

		chip8.tick();

		if(debug){
			printf("\033c");
			
			for(int i = 0; i < 16; i++){
				std::cout << chip8.keyStates[i];
			}
			std::cout << std::endl;
			std::cout << fmt::format("Delay Timer: {:X}  Sound Timer: {:X}",chip8.delayTimer,chip8.soundTimer) << std::endl;
			std::cout << fmt::format("0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F") << std::endl;
			for(int i = 0; i < 0x10; i++){
				std::cout << fmt::format("[{:02X}] ",chip8.V[i]);
			}
			std::cout << std::endl;

			std::string* log = chip8.logger.get(5);
			std::cout << fmt::format("{:X}",chip8.getAddress()) << std::endl;

			for(int i = 0; i < 5; i ++){
				std::cout << log[i] << std::endl;
			}

			std::cout << "Stack Pointer: " << chip8.stackPointer << std::endl;

			std::cout << "Stack:" << std::endl;
			for(int i = 0; i < chip8.stackPointer; i++){
				std::cout << fmt::format("{:X}",chip8.stack[i]) << std::endl;
			}

		}
		//if(chip8.drawFlag){
			// Set the screen back to black (we do this every frame before redrawing everything)
			window.clear(sf::Color::Black);

			// Draw all pixels from CHIP-8 graphics buffer
			// TODO: Fix this, no need to redraw the scren after every tick only on 0x00E0 and 0xDXYN
			
		
			//i = x + width*y
			for(int i = 0; i < 2048; i++){
				if(chip8.getPixel(i)){
					pixel.setPosition((i%64),(i/64));
					window.draw(pixel);
				}
			}
			chip8.drawFlag = false;
		//}

		/*
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 32; y++) {
				if (chip8.getPixel(x, y)) {
					pixel.setPosition(x, y);
					window.draw(pixel);
				}
			}
		}*/

		// Display changes
		window.display();

		// Sound buzzer if necessary
		if (chip8.isBuzzer())
			beep.play();
	}

	return 0;
}

// Converts an SFML keycode to a CHIP-8 hexadecimal keycode. Returns -1 if the SFML keycode has no CHIP-8 equivalent.
int convertKeycode(sf::Keyboard::Key key)
{


	switch (key) {
	// Row 0
	case sf::Keyboard::Num1: return 0x1;
	case sf::Keyboard::Num2: return 0x2;
	case sf::Keyboard::Num3: return 0x3;
	case sf::Keyboard::Num4: return 0xC;

	// Row 1
	case sf::Keyboard::Q: return 0x4;
	case sf::Keyboard::W: return 0x5;
	case sf::Keyboard::E: return 0x6;
	case sf::Keyboard::R: return 0xD;

	// Row 2
	case sf::Keyboard::A: return 0x7;
	case sf::Keyboard::S: return 0x8;
	case sf::Keyboard::D: return 0x9;
	case sf::Keyboard::F: return 0xE;

	// Row 3
	case sf::Keyboard::Z: return 0xA;
	case sf::Keyboard::X: return 0x0;
	case sf::Keyboard::C: return 0xB;
	case sf::Keyboard::V: return 0xF;

	default: return -1;
	}
}
int charKeycode(char key)
{


	switch (key) {
	// Row 0
	case '1': return 0x1;
	case '2': return 0x2;
	case '3': return 0x3;
	case '4': return 0xC;

	// Row 1
	case 'q': return 0x4;
	case 'w': return 0x5;
	case 'e': return 0x6;
	case 'r': return 0xD;

	// Row 2
	case 'a': return 0x7;
	case 's': return 0x8;
	case 'd': return 0x9;
	case 'f': return 0xE;

	// Row 3
	case 'z': return 0xA;
	case 'x': return 0x0;
	case 'c': return 0xB;
	case 'v': return 0xF;

	default: return -1;
	}
}
