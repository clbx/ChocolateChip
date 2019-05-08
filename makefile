
IMGUI = ./include/imgui_demo.cpp ./include/imgui_draw.cpp ./include/imgui_sdl.cpp ./include/imgui_widgets.cpp ./include/imgui.cpp

default:
	g++ -g src/main.cpp src/Chocolate.cpp -Wall -std=c++14 -stdlib=libc++ -I./include -lSDL2 -o "chocolate"

test: g++ -g src/test.cpp -Wall -std=c++13 -stdlib=libc++ -I./include -lSDL2 -o "chocolate"