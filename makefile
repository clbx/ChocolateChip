
IMGUI = ./include/imgui_demo.cpp ./include/imgui_draw.cpp ./include/imgui_sdl.cpp ./include/imgui_widgets.cpp ./include/imgui.cpp

default:
	g++ -g src/main.cpp $(IMGUI) -Wall -std=c++14 -stdlib=libc++ -I./include -lSDL2 -o "chocolate"