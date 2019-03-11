CXX = g++

INCLUDE_LOCATION = -I./include
LIB_LOCATION = -L./lib
LIBS = -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system

CXXFLAGS = -Wall -std=c++11 $(INCLUDE_LOCATION) $(LIB_LOCATION) $(LIBS)

default:
	$(CXX) -g src/Application.cpp src/Chocolate.cpp src/Logger.cpp $(CXXFLAGS) -o "chocolate"
test: 
	$(CXX) -g src/ChocolateTests.cpp src/Chocolate.cpp src/Logger.cpp $(CXXFLAGS) -lgtest -o "chocolate"
temp:
	$(CXX) -g src/TempMain.cpp src/Chocolate.cpp src/Logger.cpp $(CXXFLAGS) -lgtest -o "chocolate"
clean:
	rm -f chip8
