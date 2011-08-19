#include "Map.h"
#include "Bot.h"
#include "Ant.h"
#include "globals.h"

#include <iostream>

Location& Loc(int row, int col) {
    if (row < 0) row += state.rows;
    if (col < 0) col += state.cols;

    if (row >= state.rows) row -= state.rows;
    if (col >= state.cols) col -= state.cols;

    return *gameMap.locationGrid[LocToIndex(row, col)];
}

Location& Loc(const Location& loc) {
    return *gameMap.locationGrid[LocToIndex(loc.row, loc.col)];
}


void Map::onInit() {
    #ifdef __DEBUG
    logger.debugLog << "Map::onInit()" << endl;
    #endif

    locNull.row = -1;
    locNull.col = -1;

    locationGrid.resize(state.rows*state.cols);
    for (int i = 0; i < state.rows*state.cols; i += 1) {
        locationGrid[i] = new Location();
        locationGrid[i]->row = IndexToRow(i);
        locationGrid[i]->col = IndexToCol(i);
        #ifdef __DEBUG
        if (i != (*locationGrid[i]).getIndex()) logger.logError("Map::onInit(): Error while indexing grid");
        #endif
    }
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
    state.ants_grid[loc.row][loc.col] = (intptr_t)ant;
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
    double d1 = abs((double)loc1.row-(double)loc2.y),
           d2 = abs((double)loc1.col-(double)loc2.x),
           dr = min(d1, (double)state.rows-d1),
           dc = min(d2, (double)state.cols-d2);
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
