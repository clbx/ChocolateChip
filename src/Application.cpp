#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Chip8.hpp"

int convertKeycode(sf::Keyboard::Key);

int main()
{
	// Load CHIP-8 buzzer sound


	char rom[64];
	printf("Enter ROM file path: ");
	scanf("%s",rom);

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("res/beep.wav")) {
		std::cerr << "Unable to load buzzer sound file, exiting" << std::endl;
		return -1;
	}
	sf::Sound beep;
	beep.setBuffer(buffer);
	beep.play(); // Startup test beep (why not?)



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
	Chip8 chip8(rom);

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
		chip8.tick();

		// Set the screen back to black (we do this every frame before redrawing everything)
		window.clear(sf::Color::Black);

		// Draw all pixels from CHIP-8 graphics buffer
		// TODO: Fix this, no need to redraw the scren after every tick only on 0x00E0 and 0xDXYN
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 32; y++) {
				if (chip8.getPixel(x, y)) {
					pixel.setPosition(x, y);
					window.draw(pixel);
				}
			}
		}

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
