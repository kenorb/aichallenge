#ifndef BOT_H_
#define BOT_H_

#include "State.h"

#ifdef __DEBUG
#include "json/json.h"
#endif

struct Bot
{
    State state;

    #ifdef __DEBUG
    Json::StyledWriter debugOutput;
    Json::Value debugNode;
    #endif

    Bot();

    void playGame();

    void debugData();
    void makeMoves();
    void endTurn();
};

#endif //BOT_H_
