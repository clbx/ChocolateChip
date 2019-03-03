CXX = g++

INCLUDE_LOCATION = -I./include
LIB_LOCATION = -L./lib
LIBS = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system

CXXFLAGS = -Wall -std=c++11 $(INCLUDE_LOCATION) $(LIB_LOCATION) $(LIBS)

default:
	$(CXX) -g src/*.cpp $(CXXFLAGS) -o "chip8"

clean:
	rm -f chip8
