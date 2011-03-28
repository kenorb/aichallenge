#include "json/json.h"
#include "AI/State.h"

#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED


struct State;

struct Logger
{
    Logger();
    ~Logger();

    void logPreState(State* state, bool init);
    void logPostState(State* state);

    Json::StyledWriter debugOutput;
    Json::Value debugNode;

    Bug jsonLog;
    Bug debugLog;

};


#endif // LOGGER_H_INCLUDED
