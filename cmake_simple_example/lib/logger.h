#pragma once

#include <string>


#ifdef WIN32
    #ifdef logger_EXPORTS
        // We are building this library
        #define LOGGER_EXPORT __declspec(dllexport)
    #else
        // We are using this library
        #define LOGGER_EXPORT __declspec(dllimport)
    #endif
#else
    // define as nothing
    #define LOGGER_EXPORT
#endif

class LOGGER_EXPORT Logger {
public:
    void logMsg(std::string msg);
};
