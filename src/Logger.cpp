#include "Logger.hpp"
#include <string>

Logger::Logger(){
    int length = 0;
    std::string log[512];
}

//TODO: Fix, this is slow af. Change to circular array

/**
 * @brief Stores something into the log
 * 
 * @param msg Message to log
 */
void Logger::store(char* msg){
    // Store until we get 512 messages
    std::string tmp(msg);
    if(length < 512 - 1){
        log[length] = tmp;
        length++;
    }
    //Otherwise move them all down one and then copy it in
    else{
        //[511] -> [510] ... [1] -> [0]
        for(int i = 511; i > 1; i--){
            log[i-1]=log[i];           
        }
        log[length]=tmp;
    }
}

/**
 * @brief Gets the last `amt` messages
 * 
 * @param amt the amount of messages to fetch
 */
std::string* Logger::get(int amt){

    std::string list[amt];

    for(int i = 0; i < amt; i++){
        int index = length - i;
        if(i < 1){
            return list;
        }
        list[i] = log[i];
    }
    return list;
}
