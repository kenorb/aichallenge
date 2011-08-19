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

// TODO:
// Time Remaining:
//  * Provide a function to return the number of milliseconds left since the last 'go' was received from the game engine.

Bot::Bot()
{
    #ifdef __DEBUG
    totalThinkTime = 0;
    longestThinkTime = 0;
    checkSum = 0;
    #endif
}

#ifdef __DEBUG
int Bot::getCheckSum() {
    return bot.checkSum % 999;
}
#endif

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

void Bot::endGame()
{
    #ifdef __DEBUG
    if (state.turn < 10) logger.logWarning("Game shorter than 10 turns?");
    logger.debugLog << endl <<
    "GAME END" << endl <<
    " + Errors: " << (int)logger.errors << endl <<
    " + Warnings: " << (int)logger.warnings << endl <<
    " + Structural ants: " << (int)state.structuralAnts.size() << endl <<
    " + Total think time: " << bot.totalThinkTime << "s" << endl <<
    " + Avg think time left: " << ((double)(state.turntime / 1000) - (bot.totalThinkTime / state.turn)) << "s" << endl <<
    " + Time utilized: " << abs(1 - (1 / ((double)(state.turntime / 1000) - (bot.totalThinkTime / state.turn))))*100 << "%" << endl <<
    " + Longest think time: " << longestThinkTime << "s" << endl <<
    " + AI memory usage: " << (profiler.aiMemoryUsage) << " bytes" << endl <<
    " + Locations created: " << (logger.locationsCreated) << endl <<
    " + Game checksum: " << bot.getCheckSum() << endl;
    #endif
}

bool Bot::hasAggressiveMode()
{
    if (bot.getExpandForce() <= 0) return true;
    if (state.visibilityCoverage > 0.80 && state.enemyAnts.size() <= state.ants.size() / 2) return true;
    return false;
}

double Bot::getExpandForce()
{
    // Based on area coverage
    double percent = 0.6;
    percent += ((double)state.ants.size() / 500);
    double ret = (percent - state.areaCoverage) * 10;
    if (abs(ret) > 1) ret = 1 * sign(ret);
    if (ret < 0.1) ret = 0.1;
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
    logger.debugLog << "START LOCATION: " << LocationToString(bot.startLocation) << endl;
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

void Bot::addChecksumValue(uint32_t val) {
    if (val <= 0) return;
    bot.checkSum += val;
}

Ant* getAnt(int id)
{
    Location& loc = state.ants[id];
    Ant* ret = gameMap.getAntAt(loc);
    #ifdef __DEBUG
    bot.addChecksumValue(state.ants[id].row+state.ants[id].col);
    if (!ret) logger.logError("Structural ant at location not found");
    #endif
    ret->updateTemporaryId(id);
    return ret;
}

bool funcSortAnts(const Location& d1, const Location& d2)
{
    //Ant* ant1 = gameMap.getAntAt(d1);
    //Ant* ant2 = gameMap.getAntAt(d2);

    // TODO: Sort ants to move the most important ones first
    return 0;
}

void Bot::makeMoves()
{
    if (state.turn == 1) firstMove();

    #ifdef __DEBUG
    logger.debugLog << "ACTION: makeMoves()" << endl;
    logger.debugLog << "Expand force: " << getExpandForce() << endl;
    logger.debugLog << "Aggressive mode: " << hasAggressiveMode() << endl;
    #endif



    // Let ants think before they move
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAnt(ant_id);
        ant->prepareMove();
    }

    #ifdef __DEBUG
    logger.logMapState();
    #endif

    // Can be used to sort ants
    // sort(state.ants.begin(), state.ants.end(), funcSortAnts);
    // though we are now using solver with ant priority

    // Ask ants to move
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAnt(ant_id);
        ant->onThink();
    }
}

void Bot::updateMap()
{
    state.updateFogOfWar();
}

void Bot::onThink()
{
    #ifdef __DEBUG
    bot.addChecksumValue(state.turn);

    double timeStart = unix_time();
    logger.debugLog << " --- BEGIN: Bot::onThink() --- " << endl;
    logger.logPreState(false);

    profiler.beginMemoryProfiling();
    #endif

    validateAnts();
    updateMap();
    makeMoves();


    #ifdef __DEBUG
    profiler.endMemoryProfiling();

    if ((int)state.ants.size() != (int)gameMap.getAnts().size()) {
        logger.logError("Number of ants given is not equal to structural ants");
    }

    double timeTaken = unix_time() - timeStart;
    if (timeTaken > longestThinkTime) longestThinkTime = timeTaken;
    totalThinkTime += timeTaken;

    logger.logPostState();

    logger.debugLog << " --- ENDOF: Bot::onThink() --- (+" << profiler.lastNewBytes << " bytes)" << endl;
    #endif
};

void Bot::endTurn()
{
    state.reset();
    state.turn++;

    cout << "go" << endl;
};
