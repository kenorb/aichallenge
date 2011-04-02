#include "Bot.h"
#include "Ant.h"
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __DEBUG
#include "Logger.h"
#endif

#include "globals.h"

#include <vector>
#include <string>
#include <algorithm>

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
    // Based on area coverage
    double percent = 0.5;
    percent += ((double)state.ants.size() / 400);
    double ret = (percent - state.areaCoverage) * 10;
    if (abs(ret) > 1) ret = 1 * sign(ret);
    return ret;
}

void Bot::firstMove()
{
    #ifdef __DEBUG
    logger.debugLog << "ACTION: firstMove()" << endl;
    #endif

    std::list<Ant*>::iterator iter_ant = gameMap.getAnts().begin();
    startLocation = (*iter_ant)->getLocation();

    #ifdef __DEBUG
    logger.debugLog << "START LOCATION: " << bot.startLocation.str() << endl;
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
    ret->updateTemporaryId(id);
    return ret;
}

bool funcSortAnts(const Location& d1, const Location& d2)
{
    Ant* ant1 = gameMap.getAntAt(d1);
    Ant* ant2 = gameMap.getAntAt(d2);

    // TODO: Sort ants to move the most important ones first
    return 0;
}

void Bot::makeMoves()
{
    if (state.turn == 1) firstMove();
    #ifdef __DEBUG
    logger.debugLog << "ACTION: makeMoves()" << endl;
    logger.debugLog << "Expand force: " << getExpandForce() << endl;
    #endif


    // Let ants think before they move
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAnt(ant_id);
        ant->prepareMove();
    }

    // Can be used to sort ants
    // sort(state.ants.begin(), state.ants.end(), funcSortAnts);

    // Ask ants to move
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAnt(ant_id);
        ant->onThink();
    }
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
