#include <string>
#include "Chocolate.hpp"
#include <fmt/core.h>

using namespace std;


int main(){

    std::string temp = fmt::format("Got : {:X} and {}", 0xA5, 0xA5);
    std::cout << temp << std::endl;

}