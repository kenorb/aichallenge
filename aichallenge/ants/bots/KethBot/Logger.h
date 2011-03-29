#include "json/json.h"
#include "AI/State.h"

#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED


struct State;

struct Logger
{
    Logger();
    ~Logger();


    void logPreState(bool init);
    void logPostState();
    void logMapState();

    void logError(std::string error);
    void logWarning(std::string warning);

    Json::StyledWriter debugOutput;
    Json::Value debugNode;

    Bug jsonLog;
    Bug debugLog;

    int errors, warnings;
};


#endif // LOGGER_H_INCLUDED
