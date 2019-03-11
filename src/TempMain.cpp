#include <string>
#include "Chocolate.hpp"
#include "Logger.hpp"

using namespace std;


int main(){
    Logger logger;
    logger.store("Hello world");
    logger.store("Goodbye World");
    logger.store("Ayo");


    cout << logger.log[0] << endl;
    cout << logger.log[1] << endl;
    cout << logger.log[2] << endl;
    cout << logger.length << endl;
    int len = 2;


    string* recv = logger.get(len);

    for(int i = 0; i < len; i++){
        cout << recv[i] << endl;
    }

}