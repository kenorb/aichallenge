#ifndef BOT_H_
#define BOT_H_

#include "State.h"


struct Bot
{
    State state;

    Bot();

    void playGame();
    void onThink();
    void endTurn();

    void firstMove();
    void makeMoves();

    void validateAnts();
    void updateMap();

    void debugData();



    void Log(const std::string &text);
};

#endif //BOT_H_
