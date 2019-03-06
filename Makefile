CXX = g++

INCLUDE_LOCATION = -I./include
LIB_LOCATION = -L./lib
LIBS = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system

CXXFLAGS = -Wall -std=c++11 $(INCLUDE_LOCATION) $(LIB_LOCATION) $(LIBS)

default:
	$(CXX) -g src/Application.cpp src/Chocolate.cpp $(CXXFLAGS) -o "chocolate"
test: 
	$(CXX) -g src/ChocolateTests.cpp src/Chocolate.cpp $(CXXFLAGS) -lgtest -o "tests"


clean:
	rm -f chip8
