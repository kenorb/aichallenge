#include "Map.h"
#include "Bot.h"
#include "Ant.h"
#include "globals.h"
#include "Optimizer.h"

#include <iostream>

const Location& Loc(int row, int col) {
    while (row < 0) row += state.rows;
    while (col < 0) col += state.cols;

    while (row >= state.rows) row -= state.rows;
    while (col >= state.cols) col -= state.cols;

    return *gameMap.locationGrid[LocToIndex(row, col)]; // loc to index inline?
}

const Location& Loc(const Location& loc) {
    return *gameMap.locationGrid[LocToIndex(loc.row, loc.col)];
}

void Map::onInit() {
    #ifdef __DEBUG
    logger.debugLog << "Map::onInit()" << endl;
    #endif

    locNull = new Location();
    locNull->row = -1;
    locNull->col = -1;

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

vector<const Location*> Map::findMany(const Location& loc, double searchRadius, LocationType type)
{
    vector<const Location*> ret;

    for (int i = 0; i < optimizer.radiusAreaMap.size(); i++) {
        const relativeLocation* relLoc = optimizer.radiusAreaMap[i];

        if (relLoc->distance > searchRadius) break;

        const Location* l = &Loc(loc.row + relLoc->row, loc.col + relLoc->col);
        #ifdef __DEBUG
        if (!l->isValid()) {
            logger.logError("Map::find - !l->isValid()");
            logger.debugLog << "DEBUG: " << LocationToString(*l) << std::endl;
        }
        #endif
        if (l->isType(type)) {
            ret.push_back(l);
        }
    }

    return ret;
}

MapSearch Map::find(const Location& loc, double searchRadius, LocationType type, MapSearch startFrom /* = MapSearch() */)
{
    MapSearch ret;

    for (int i = startFrom.index + 1; i < optimizer.radiusAreaMap.size(); i++) {
        ret.index = i;
        const relativeLocation* relLoc = optimizer.radiusAreaMap[i];

        if (relLoc->distance > searchRadius) {
            ret.reachedRadius = true;
            break;
        }

        const Location* l = &Loc(loc.row + relLoc->row, loc.col + relLoc->col);
        #ifdef __DEBUG
        if (!l->isValid()) {
            logger.logError("Map::find - !l->isValid()");
            logger.debugLog << "DEBUG: " << LocationToString(*l) << std::endl;
        }
        #endif
        if (l->isType(type)) {
            ret.location = l;
            ret.found = true;
            break;
        }
    }

    return ret;
}

void Map::callbackArea(const Location& loc, double radius, CallbackLoc callback, const void* sender /* = NULL */)
{
    if (radius > OPTIMIZER_MAX_RADIUS) {
        #ifdef __DEBUG
        logger.logError("Map::callbackArea(): Radius not computed");
        #endif
        return;
    }

    int r = round(sqr(radius));

    //logger.debugLog << "optimizer.radiusArea[" <<s r << "].size() = " << optimizer.radiusArea[r].size() << std::endl;

    for (int i = 0; i < optimizer.radiusArea[r].size(); i++) {
        const relativeLocation* relLoc = optimizer.radiusArea[r][i];
        const Location& l = Loc(loc.row + relLoc->row, loc.col + relLoc->col);

        CallbackLocData data;
        data.sender = sender;
        data.senderLocation = &loc;
        data.relLoc = relLoc;

        callback(l, data);
    }
}

void Map::onEnemy(const Location &loc)
{
    // logger.debugLog << "EVENT: Enemy found at " << loc.str() << endl;
}

void Map::onAnt(int bot_id, const Location &loc)
{
    if (bot_id == 0) {
        if (!getAntAt(loc)) setAntAt(loc, new Ant(state, loc));
    } else {
        onEnemy(loc);
    }
}

void Map::onAntMoves(Ant* ant, const Location &toLocation)
{
    #ifdef __DEBUG
    if (getAntAt(toLocation)) {
        logger.logError("Moving ant to a place where there is already an ant!");;
    }
    #endif

    setAntAt(toLocation, ant);
    setAntAt(ant->getLocation(), NULL);
}

void Map::onDeadAnt(int bot_id, const Location &loc)
{
    // Works but should not be used as it might not always be accurate
}

void Map::onFood(const Location &loc)
{
    // On food found
}

void Map::onWater(const Location &loc)
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
    return (Ant*)state.ants_grid[loc.row][loc.col];
}

Ant* Map::setAntAt(const Location &loc, Ant* ant)
{
    state.ants_grid[loc.row][loc.col] = (intptr_t)ant;
    return ant;
}

double Map::distance(int row1, int col1, int row2, int col2) {
    return distance_fast(row1, col1, row2, col2);
}

double Map::distance(const Location &loc1, const Location &loc2)
{
    #ifdef __DEBUG
    #ifndef __OPTIMIZER_TEST
    profiler.beginThinkTime(TT_DISTANCES);
    #endif
    #endif

    double ret = distance_fast(loc1.row, loc1.col, loc2.row, loc2.col);

    #ifdef __DEBUG
    #ifndef __OPTIMIZER_TEST
    profiler.endThinkTime(TT_DISTANCES);
    #endif
    #endif

    return ret;
}

double Map::distance_sqrt(int row1, int col1, int row2, int col2)
{
    int d1 = row1-row2,
    d2 = col1-col2;

    if (d1 < 0) d1 = -d1;
    if (d2 < 0) d2 = -d2;

    int dr = state.rows-d1;
    if (d1 < dr) dr = d1;

    int dc = state.cols-d2;
    if (d2 < dc) dc = d2;

    return sqrt(dr*dr + dc*dc);
}

double Map::distance_sqrt(const Location &loc1, const Location &loc2)
{
    return distance_sqrt(loc1.row, loc1.col, loc2.row, loc2.col);
}

double Map::distance_lut(const Location &loc1, const Location &loc2)
{
    int d1 = loc1.row-loc2.row,
        d2 = loc1.col-loc2.col;

    if (d1 < 0) d1 = -d1;
    if (d2 < 0) d2 = -d2;

    int dr = state.rows-d1;
    if (d1 < dr) dr = d1;

    int dc = state.cols-d2;
    if (d2 < dc) dc = d2;

    return optimizer.sqrt_table[dr*dr+dc*dc];
}

double Map::distance_table(const Location &loc1, const Location &loc2)
{
    return distance_fast(loc1.row, loc1.col, loc2.row, loc2.col);
}

double Map::distance_vector(const Location &loc1, vector2f loc2)
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_DISTANCES);
    #endif

    double d1 = abs((double)loc1.row-(double)loc2.y),
           d2 = abs((double)loc1.col-(double)loc2.x),
           dr = min(d1, (double)state.rows-d1),
           dc = min(d2, (double)state.cols-d2);

    return sqrt(dr*dr + dc*dc);

    #ifdef __DEBUG
    profiler.endThinkTime(TT_DISTANCES);
    #endif
}
