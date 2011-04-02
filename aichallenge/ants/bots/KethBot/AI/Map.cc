#include "Map.h"
#include "Bot.h"
#include "Ant.h"

#include <iostream>

Map::Map()
{

}

void Map::onEnemy(Location &loc)
{
    // logger.debugLog << "EVENT: Enemy found at " << loc.str() << endl;
}

void Map::onAnt(int bot_id, Location &loc)
{
    if (bot_id == 0) {
        if (!getAntAt(loc)) setAntAt(loc, new Ant(state, loc));
    } else {
        onEnemy(loc);
    }
}

void Map::onAntMoves(Ant* ant, Location &toLocation)
{
    #ifdef __DEBUG
    if (getAntAt(toLocation)) {
        logger.logError("Moving ant to a place where there is already an ant!");;
    }
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

std::list<Ant*>& Map::getAnts()
{
    return state.structuralAnts;
}

vector<Location>& Map::getEnemyAnts()
{
    return state.enemyAnts;
}

Ant* Map::getAntAt(const Location &loc)
{
    if (state.ants_grid[loc.row][loc.col]) {
        if (Ant* ant = (Ant*)state.ants_grid[loc.row][loc.col]) {
            return ant;
        }
    }
    return NULL;
}

Ant* Map::setAntAt(const Location &loc, Ant* ant)
{
    state.ants_grid[loc.row][loc.col] = (int)ant;
    return ant;
}

double Map::distance(const Location &loc1, const Location &loc2)
{
    int d1 = abs(loc1.row-loc2.row),
        d2 = abs(loc1.col-loc2.col),
        dr = min(d1, state.rows-d1),
        dc = min(d2, state.cols-d2);
    return sqrt(dr*dr + dc*dc);
}

double Map::distance_vector(const Location &loc1, vector2f loc2)
{
    double d1 = abs((double)loc1.row-loc2.y),
           d2 = abs((double)loc1.col-loc2.x),
           dr = min(d1, state.rows-d1),
           dc = min(d2, state.cols-d2);
    return sqrt(dr*dr + dc*dc);
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
