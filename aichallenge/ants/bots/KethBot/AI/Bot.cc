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
    checkSum = 0;
    codeDepth = 0;
    #endif
}

#ifdef __DEBUG
int Bot::getCheckSum() {
    return bot.checkSum % 999;
}
#endif

void Bot::playGame()
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_TOTALLOADTIME);
    #endif

    #ifdef __DEBUG
    profiler.beginThinkTime(TT_GAMELOADTIME);
    #endif
    cin >> state;
    #ifdef __DEBUG
    profiler.endThinkTime(TT_GAMELOADTIME);
    #endif

    #ifdef __DEBUG
    logger.debugLog << "Optimizer init..." << std::endl;
    profiler.beginThinkTime(TT_OPTIMIZERLOADTIME);
    #endif
    optimizer.init();
    #ifdef __DEBUG
    profiler.endThinkTime(TT_OPTIMIZERLOADTIME);
    logger.debugLog << "Optimizer init done..." << std::endl;

    logger.logPreState(true);

    #endif

    #ifdef __DEBUG
    profiler.endThinkTime(TT_TOTALLOADTIME);
    #endif

    while(cin >> state)
    {
        if (state.gameover) break;
        onThink();
        endTurn();
    }

    //logger.debugLog << "test3" << std::endl;
    std::cout << "test4" << std::endl;
}

void Bot::endGame()
{
    #ifdef __DEBUG
    if (state.turn < 10) logger.logWarning("Game shorter than 10 turns?");
    logger.debugLog << endl <<
    "GAME END" << endl <<
    #ifdef __ASSERT
    " + Errors: " << (int)logger.errors << endl <<
    #endif
    " + Warnings: " << (int)logger.warnings << endl <<
    " + Structural ants: " << (int)state.structuralAnts.size() << endl <<
    " + Turns: " << (int)state.turns << endl
    << profiler.output() <<
    " + Locations created: " << (logger.locationsCreated) << endl <<
    " + AStar cache used: " << (logger.astarNodesUsed) << endl <<
    " + AStar paths made: " << (logger.astarTotalPaths) << endl <<
    " + AStar iterations: " << (logger.astarIterations) << endl <<
    " + Game checksum: " << bot.getCheckSum() << endl;
    //" + Game moves: " << state.moves.str() << endl;
    #endif
}

bool Bot::hasAggressiveMode()
{
    //if (bot.getExpandForce() <= 0) return true;
    if (state.areaCoverage >= areaCoverageGoal() && state.enemyAnts.size() <= ((double)(state.ants.size()) / 2.0f)) return true;
    return false;
}

double Bot::areaCoverageGoal()
{
    return 0.6;
}

double Bot::getExpandForce()
{
    // Based on area coverage
    double percent = Bot::areaCoverageGoal();
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

#ifdef __DEBUG
void Bot::addChecksumValue(int val) {
    if (val < 0) val = -val;
    if (val == 0) val = 1;
    bot.checkSum += val;
}
#endif

Ant* getAntById(int id)
{
    Location& loc = state.ants[id];
    Ant* ret = gameMap.getAntAt(loc);
    #ifdef __ASSERT
    if (!ret) logger.logError("Structural ant at location not found");
    #endif
    ret->updateTemporaryId(id);
    return ret;
}

bool funcSortAnts(const Location& d1, const Location& d2)
{
    //Ant* ant1 = gameMap.getAntAt(d1);
    //Ant* ant2 = gameMap.getAntAt(d2);

    //if (ant1 && ant1->path) return true;

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

    state.sortDamageGrid();

    // Let ants think before they move
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAntById(ant_id);
        #ifdef __DEBUG
        logger.debugLog << "OnBefore " << *ant << " prepareMove" << std::endl;
        #endif
/*
        if (state.timeLeft() <= 300) {
            #ifdef __DEBUG
            logger.logWarning("300ms of time left. Canceling think loop for this turn.");
            #endif
            break; // doesnt work?
        }
*/
        ant->prepareMove();
    }

    #ifdef __DEBUG
    logger.debugLog << "[prepareMove done]" << std::endl;
    #endif

    #ifdef __JSON
    logger.logMapState();
    #endif

    // Can be used to sort ants
    // sort(state.ants.begin(), state.ants.end(), funcSortAnts);
    // though we are now using solver with ant priority

    // Ask ants to move
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAntById(ant_id);

        #ifdef __DEBUG
        logger.debugLog << "OnBefore " << *ant << " think" << std::endl;
        #endif
/*
        if (state.timeLeft() <= 500) {
            #ifdef __DEBUG
            logger.logWarning("300ms of time left. Canceling think loop for this turn.");
            #endif
            break; // doesnt work?
        }
*/
        ant->onNewTurn();
        ant->onThink();
    }

    #ifdef __DEBUG
    logger.debugLog << "[onThink done]" << std::endl;
    #endif

    #ifdef __ASSERT
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAntById(ant_id);
        for (int ant_id2 = 0; ant_id2 < (int)state.ants.size(); ant_id2++) {
            Ant* ant2 = getAntById(ant_id2);
            if (ant != ant2 && ant->path && ant2->path && ant->path->targetLocation().equals(ant2->path->targetLocation())) {
                logger.logWarning("Two ants have same path destination");
                logger.debugLog << "DEBUG: " << *ant << " & " << *ant2 << std::endl;
                logger.debugLog << "DEBUG: " << LocationToString(ant->path->targetLocation()) << " & " << LocationToString(ant2->path->targetLocation()) << std::endl;
            }
        }
    }
    #endif
}

void Bot::updateMap()
{
    state.updateFogOfWar();
}

void Bot::onThink()
{
    #ifdef __DEBUG
    bot.addChecksumValue(state.turn);

    profiler.beginThinkTime(TT_TOTAL);

    logger.debugLog << " --- BEGIN: Bot::onThink() --- " << endl;

    logger.logPreState(false);

    profiler.beginMemoryProfiling();
    #endif

    #ifdef __DEBUG
    profiler.beginThinkTime(TT_CORE);
    #endif

    validateAnts();

    #ifdef __DEBUG
    profiler.endThinkTime(TT_CORE);
    #endif

    #ifdef __DEBUG
    profiler.beginThinkTime(TT_MAP);
    #endif

    updateMap();

    #ifdef __DEBUG
    profiler.endThinkTime(TT_MAP);
    #endif

    makeMoves();

    #ifdef __DEBUG
    profiler.endMemoryProfiling();

    #ifdef __ASSERT
    if ((int)state.ants.size() != (int)gameMap.getAnts().size()) {
        logger.logError("(int)state.ants.size() != (int)gameMap.getAnts().size()");
        logger.debugLog << (int)state.ants.size() << " != " << (int)gameMap.getAnts().size() << std::endl;
    }
    #endif

    logger.logPostState();

    profiler.endThinkTime(TT_TOTAL);

    //logger.debugLog << " --- ENDOF: Bot::onThink() --- (+" << profiler.lastNewBytes << " bytes)" << endl;
    logger.debugLog << " --- ENDOF: Bot::onThink() --- " << endl;
    #endif
};

void Bot::endTurn()
{
    state.reset();
    state.turn++;

    cout << "go" << endl;
};
