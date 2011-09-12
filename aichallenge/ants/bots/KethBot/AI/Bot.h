#ifndef BOT_H_
#define BOT_H_

#include "State.h"
#include "Map.h"

struct Bot
{
    Bot();

    void playGame();
    void endGame();
    int getCheckSum();

    bool hasAggressiveMode();
    double areaCoverageGoal();
    double getExpandForce();

    void onThink();
    void endTurn();

    void firstMove();
    void makeMoves();

    void validateAnts();
    void updateMap();

    void debugData();

    Location startLocation;
    double targetDistance;

    void Log(const std::string &text);
    void addChecksumValue(int val);

    #ifdef __DEBUG
    int checkSum;
    #endif
};

Ant* getAntById(int id);

#endif //BOT_H_
