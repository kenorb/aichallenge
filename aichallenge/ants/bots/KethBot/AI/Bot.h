#ifndef BOT_H_
#define BOT_H_

#include "State.h"


struct Bot
{
    State state;

    Bot();

    void playGame();

    void debugData();
    void firstMove();
    void makeMoves();
    void endTurn();

    void Log(const std::string &text);
};

#endif //BOT_H_
