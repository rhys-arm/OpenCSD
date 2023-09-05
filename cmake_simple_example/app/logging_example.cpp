#include <iostream>
#include "logger.h"

int main() {
    std::cout << "Starting" << std::endl;

    Logger logger;
    logger.logMsg("Hello world");

    std::cout << "Done" << std::endl;
}
