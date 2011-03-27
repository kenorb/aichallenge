#include "json/json.h"
#include "AI/State.h"

#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED


struct State;

struct Logger
{
    Logger();
    ~Logger();

    void logState(State* state, bool init);

    Json::StyledWriter debugOutput;
    Json::Value debugNode;


};


#endif // LOGGER_H_INCLUDED
