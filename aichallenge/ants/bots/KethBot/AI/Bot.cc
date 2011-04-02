#include "Bot.h"
#include "Ant.h"
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __DEBUG
#include "Logger.h"
#endif

#include "globals.h"

Bot::Bot()
{

}

void Bot::playGame()
{
    cin >> state;

    #ifdef __DEBUG
    logger.logPreState(true);
    #endif
    while(cin >> state)
    {

        onThink();
        endTurn();
    }
}

double Bot::getExpandForce()
{
    double ret = (0.5 - state.areaCoverage) * 10;
    if (abs(ret) > 1) ret = 1 * sign(ret);
    return ret;
}

void Bot::firstMove()
{
    #ifdef __DEBUG
    logger.debugLog << "ACTION: firstMove()" << endl;
    #endif
}

void Bot::validateAnts()
{
    int i = 0;
    while (true) {
        i = 0;
        std::list<Ant*>::iterator iter_ant;
        for (iter_ant = gameMap.getAnts().begin(); iter_ant != gameMap.getAnts().end(); iter_ant++)
        {
            Ant* ant = (*iter_ant);
            ant->hasMoved = false;

            if (state.grid[ant->getLocation().row][ant->getLocation().col] != 'a') {
                Ant* foundAnt = gameMap.getAntAt(ant->getLocation());
                if (foundAnt) {
                    gameMap.setAntAt(foundAnt->getLocation(), NULL);
                    delete foundAnt;
                    i++;
                }
            }
            if (i > 0) break;
        }
        if (i == 0) break;
    }
}

Ant* getAnt(int id)
{
    Location loc = state.ants[id];
    Ant* ret = gameMap.getAntAt(loc);
    #ifdef __DEBUG
    if (!ret) logger.logError("Structural ant at location not found");
    #endif
    return ret;
}

void Bot::makeMoves()
{
    if (state.turn == 1) firstMove();
    #ifdef __DEBUG
    logger.debugLog << "ACTION: makeMoves()" << endl;
    logger.debugLog << "Expand force: " << getExpandForce() << endl;
    #endif

    // Let ants think before they move
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) getAnt(ant_id)->prepareMove();

    // Ask ants to move
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) getAnt(ant_id)->onThink();
}

void Bot::updateMap()
{
    state.updateFogOfWar();
    #ifdef __DEBUG
    logger.logMapState();
    #endif
}

void Bot::onThink()
{
    #ifdef __DEBUG
    logger.debugLog << " --- BEGIN: Bot::onThink() --- " << endl;
    logger.logPreState(false);
    #endif

    validateAnts();
    updateMap();
    makeMoves();

    #ifdef __DEBUG
    if ((int)state.ants.size() != (int)gameMap.getAnts().size()) {
        logger.logError("Number of ants given is not equal to structural ants");
    }

    logger.logPostState();

    logger.debugLog << " --- ENDOF: Bot::onThink() --- " << endl;
    #endif
};

void Bot::endTurn()
{
    state.reset();
    state.turn++;

    cout << "go" << endl;
};
