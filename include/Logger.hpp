#include <string>
#include <iostream>
/**
 * @brief A logger that keeps track of previously used opcodes
 * Should be turned off during normal use, can consume a lot of memory (~32MB)
 * And as of now is more intensive that it needs to be
 */

class Logger{
    public:
        Logger();
        void store(std::string);
        std::string* get(int);
        int length;
        std::string log[512];        

};