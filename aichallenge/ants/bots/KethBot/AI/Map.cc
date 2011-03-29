#include "Map.h"
#include "Bot.h"
#include "Ant.h"

#include <iostream>

Map::Map()
{
    bot.state.map = this;
    state = &(bot.state);
}

void Map::onEnemy(Location &loc)
{
    // state->logger.debugLog << "EVENT: Enemy found at " << loc.str() << endl;
}

void Map::onAnt(int bot_id, Location &loc)
{
    if (bot_id == 0) {
        if (!getAntAt(loc)) setAntAt(loc, new Ant((*state), loc));
    } else {
        onEnemy(loc);
    }
}

void Map::onAntMoves(Ant* ant, Location &toLocation)
{
    #ifdef __DEBUG
    if (getAntAt(toLocation)) state->logError("Moving ant to a place where there is already an ant!");;
    #endif

    setAntAt(toLocation, ant);
    setAntAt(ant->getLocation(), NULL);
}

void Map::onDeadAnt(int bot_id, Location &loc)
{
    // Works but should not be used as it might not always be accurate
}

void Map::onFood(Location &loc)
{
    // On food found
}

void Map::onWater(Location &loc)
{
    // Does not repeat
}

Ant* Map::getAntAt(const Location &loc)
{
    if (bot.state.ants_grid[loc.row][loc.col]) {
        if (Ant* ant = (Ant*)bot.state.ants_grid[loc.row][loc.col]) {
            return ant;
        }
    }
    return NULL;
}

Ant* Map::setAntAt(const Location &loc, Ant* ant)
{
    bot.state.ants_grid[loc.row][loc.col] = (int)ant;
    return ant;
}
/*
bool Map::locationThink(const Location& loc)
{
    if (Ant* ant = getAntAt(loc)) {
        ant->onThink();
        return true;
    }
    return false;
}*/
