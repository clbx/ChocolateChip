#include <string>
#include "Chocolate.hpp"

using namespace std;


int main(){

    //10

    //203

    int reg = (0x3ACB & 0x0F00) >> 8;
    int dat = (0x3ACB & 0x00FF);
    printf("%d\n", reg);
    printf("%d",dat);

}