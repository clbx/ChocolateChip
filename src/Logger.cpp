#include "Logger.hpp"
#include <string>

Logger::Logger(){
    length = 0;
    std::string log[64];
}

//TODO: Fix, this is slow af. Change to circular array

/**
 * @brief Stores something into the log
 * 
 * @param msg Message to log
 */
void Logger::store(std::string msg){
    // Store until we get 512 messages
    if(length < 512 - 1){
        log[length] = msg;
        length++;
    }
    
    //Otherwise move them all down one and then copy it in
    else{
        //[511] -> [510] ... [1] -> [0]
        for(int i = 511; i > 1; i--){
            log[i-1]=log[i];           
        }
        log[length]=msg;
    }
}

/**
 * @brief Gets the last `amt` messages
 * 
 * @param amt the amount of messages to fetch, max 64
 */
std::string* Logger::get(int amt){

    if(amt > 64){
        amt = 64;
    }

    static std::string list[64];

    for(int i = 0; i < amt; i++){
        int index = length - i;
        if(index < 1){
            return list;
        }
        list[i] = log[length - i -1];
    }
    return list;
}
