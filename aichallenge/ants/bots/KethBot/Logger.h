#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include "json/json.h"
#include "AI/State.h"

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

    void saveJson();

    Json::StyledWriter debugOutput;
    Json::Value debugNode;

    Bug jsonLog;
    Bug debugLog;

    int locationsCreated;
    int errors, warnings;
};

std::string LocationToString(Location& loc);


#endif // LOGGER_H_INCLUDED
