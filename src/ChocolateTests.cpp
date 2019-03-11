#include <string>
#include <gtest/gtest.h>
#include "Chocolate.hpp"
#include "Logger.hpp"

TEST(Logger, AddMessages)
{   
    Logger logger;
}


int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}