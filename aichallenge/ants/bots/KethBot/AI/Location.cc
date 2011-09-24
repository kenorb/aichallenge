#include <algorithm>
#include <string.h>

#include "Location.h"
#include "Ant.h"
#include "Bot.h"
#include "Magnets.h"
#include "Optimizer.h"

#include "globals.h"
#include "allocator.h"



Location::Location(int r, int c)
{
    row = r;
    col = c;

    #ifdef __DEBUG
    logger.locationsCreated++;
    #endif
}

Location::Location(vector2f vec)
{
    row = round(vec.y);
    col = round(vec.x);

    while (row < 0) row += state.rows;
    while (col < 0) col += state.cols;

    while (row >= state.rows) row -= state.rows;
    while (col >= state.cols) col -= state.cols;

    #ifdef __DEBUG
    logger.locationsCreated++;
    #endif
}

int Location::getIndex() {
    return LocToIndex(row, col);
}

void Location::onUpdate() const {
    // Executed every new turn
}

void Location::onAppear() const {
    // Executed every time location appears in vision
    // Keep in mind already visible locations will not call this function
    // so e.g. food that appears in already visible location, this event will not catch it.
}

void Location::onDisappear() const {
    // Executed every time location appears in vision
}


void Location::normalize()
{
    while (row < 0) row += state.rows;
    while (col < 0) col += state.cols;

    while (row >= state.rows) row -= state.rows;
    while (col >= state.cols) col -= state.cols;
}

Damage& Location::damageArea() const {
    return state.gridDamage[this->index];
}

bool Location::think() const
{
    if (Ant* ant = gameMap.getAntAt(*this)) {
        ant->onThink();
        return true;
    }
    return false;
}

#ifdef __DEBUG
void Location::addDebugLine(std::string line)
{
    char turn[15];
    std::stringstream os;
    sprintf(turn, "%03i", state.turn);

    char pos[32];
    sprintf(pos, "#text_%ix%i", row, col);

    logger.debugNode[turn][pos] = line;
}
#endif

bool Location::isValid() const
{
    if (this == locNull) {
        return false;
    }

    #ifdef __ASSERT
    if (row < 0 || col < 0 || row > state.rows || col > state.cols) {
        logger.logError("Invalid location");
        logger.debugLog << "Debug output: " << LocationToString(*this) << std::endl;
        return false;
    }
    #endif

    return true;
};

Ant* Location::getAnt() const
{
    if (!isValid()) return NULL;
    return gameMap.getAntAt(*this);
}

bool Location::hasAnt() const
{
    return gameMap.getAntAt(*this) != NULL;
}

bool Location::isType(const LocationType type) const
{
    char s = state.grid[row][col];
    switch (type) {
        case ANT: return s == 'a' || s == 'b';
        case ANT_FRIENDLY: return s == 'a';
        case WALL: return s == '%';
        case ANT_ENEMY: return s == 'b';
        case FOOD_FOCUSED: return s == '^';
        case FOOD_BLURRED: return s == '*';
        case FOOD: return s == '*' || s == '^';
        case EMPTY: return s == '.' || s == 'o';
        case FOG: return s == '?';
        #ifdef __ASSERT
        default:
            logger.logError("Location::isType - Unknown LocationType");
            logger.debugLog << "DEBUG: type = " << type << std::endl;
        break;
        #endif

    }
    return false;
}

bool Location::isAround(const Location& loc) const
{
    return distance_fast((*this).row, (*this).col, loc.row, loc.col) <= state.spawnradius;
}

bool funcSortSearchLocations(SearchLocation* loc1, SearchLocation* loc2)
{
    //if (loc1->cost == loc2->cost) {
        //return loc1->distanceLeft < loc2->distanceLeft;
    //}
    return (loc1->cost) > (loc2->cost);
}

Path* Location::findPathTo(const Location& endLocation, bool costOnly /* = false */) const
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_ASTAR);
    logger.astarTotalPaths++;
    #endif

    Path* path = new(gameMap.pathPool->Acquire())Path;

    const Location& startLocation = *this;

    path->setSource(startLocation);
    path->setTarget(endLocation);

    double distance = distance_fast(startLocation.row, startLocation.col, endLocation.row, endLocation.col);

    if (distance <= state.spawnradius) {
        path->totalCost = distance;

        #ifdef __DEBUG
        profiler.endThinkTime(TT_ASTAR);
        logger.astarTotalPaths--;
        #endif

        return path;
    }

    gameMap.opened.clear();

    memset(gameMap.search_grid, 0, sizeof(SearchLocation) * state.rows * state.cols);

    SearchLocation& sl = gameMap.search_grid[this->index];
    sl.loc = this;
    sl.cost = 0;
    //sl.distanceLeft = distance_fast(row, col, endLocation.row, endLocation.col);

    gameMap.opened.push_back(&sl);

    while (gameMap.opened.size() > 0) {
        std::sort(gameMap.opened.begin(), gameMap.opened.end(), funcSortSearchLocations);
        SearchLocation* openedSquare = gameMap.opened.back();
        gameMap.opened.pop_back();

        openedSquare->opened = false;

        for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)
        {
            // Don't let diagonal movement
            if (x != 0 && y != 0) continue;
            if (x == 0 && y == 0) continue;

            const Location* currentLocation = &LocWrap(Row(openedSquare->loc) + y, Col(openedSquare->loc) + x);

            if (!path->touchedFog && LocIsFog(currentLocation->row, currentLocation->col)) path->touchedFog = true;
            SearchLocation& searchLocation = gameMap.search_grid[currentLocation->index];


            if (searchLocation.loc != NULL) continue;

            //int distance = distance_fast(currentLocation->row, currentLocation->col, endLocation.row, endLocation.col);

            searchLocation.loc = currentLocation;
            searchLocation.cost = openedSquare->cost + 1;
            //searchLocation.distanceLeft = distance;
            searchLocation.ref = openedSquare->loc;

            uint8& costCached = optimizer.distance_cost_table[startLocation.index][currentLocation->index];

            if (searchLocation.cost < costCached || costCached == 0) {
                costCached = searchLocation.cost;
            }

            if (currentLocation->row == endLocation.row && currentLocation->col == endLocation.col) {
                path->totalCost = searchLocation.cost;

                while (not (currentLocation->row == startLocation.row && currentLocation->col == startLocation.col)) {
                    path->moves.push(currentLocation);
                    currentLocation = gameMap.search_grid[currentLocation->index].ref;

                    const Location* loopLocation = currentLocation;
                    while (not (loopLocation->row == startLocation.row && loopLocation->col == startLocation.col)) {
                        loopLocation = gameMap.search_grid[loopLocation->index].ref;
                        uint8& costCached = optimizer.distance_cost_table[currentLocation->index][loopLocation->index];
                        costCached = gameMap.search_grid[currentLocation->index].cost - gameMap.search_grid[loopLocation->index].cost;
                    }
                }

                #ifdef __DEBUG
                profiler.endThinkTime(TT_ASTAR);
                #endif

                return path;
            }

            if (LocIsWall(currentLocation->row, currentLocation->col)) continue;
            gameMap.opened.push_back(&searchLocation);

            #ifdef __DEBUG
            logger.astarIterations++;
            #endif
        }
    }

    gameMap.pathPool->Release(path);

    #ifdef __DEBUG
    profiler.endThinkTime(TT_ASTAR);
    #endif

    return NULL;
}

double Location::costTo(const Location& loc, bool precise /* = FALSE */) const
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_ASTAR);
    #endif

    double ret;
    precise = true;

    int computedCost = cachedNode(this->index, loc.index);

    #ifdef __ASSERT
    logger.debugLog << "costFrom " << LocationToString(*this) << " to " << LocationToString(loc) << std::endl;
    #endif

    if (computedCost > 0) {
        #ifndef __ASSERT
        #ifdef __DEBUG
        profiler.endThinkTime(TT_ASTAR);
        #endif
        #endif

        #ifdef __DEBUG
        logger.astarNodesUsed++;
        #endif

        #ifndef __ASSERT
        return computedCost;
        #else
        logger.debugLog << "costTo cache = " << computedCost << std::endl;
        #endif
    }

    Path* path = Location::findPathTo(loc, true);
    if (path) {
        ret = path->totalCost;
    } else {
        ret = numeric_limits<double>::max();
    }

    #ifdef __DEBUG
    profiler.endThinkTime(TT_ASTAR);
    #endif

    #ifdef __ASSERT
    logger.debugLog << "costTo result = " << ret << std::endl;
    if (computedCost > 0 && computedCost != ret) {
        if (path->touchedFog) {
            logger.logWarning("WARNING: computedCost > 0 && computedCost != ret && touchedFog");
        } else {
            logger.logError("ERROR: computedCost > 0 && computedCost != ret");
        }
    }
    #endif

    gameMap.pathPool->Release(path);

    return ret;
}

DamageSolve Location::solveAttack()
{
    DamageSolve damageSolve;
    //friendlyAnts = findMany(const Location& loc, double searchRadius, LocationType type)
}

/*
foodList Location::foodInRadius(double radius) const
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_NEAREST_FOOD);
    #endif

    foodList list;
    for(int _row=0; _row<state.rows; _row++)
    for(int _col=0; _col<state.cols; _col++)
    {
        if(state.grid[_row][_col] == '*') {
            double dist = distance_fast(row, col, _row, _col);
            if (dist <= radius) {
                Food food;
                food.location = Loc(_row, _col);
                food.attractionForce = food.location.getForce(NULL, true, false).length();
                list.push_back(food);
            }
        }
    }

    #ifdef __DEBUG
    profiler.endThinkTime(TT_NEAREST_FOOD);
    #endif

    //std::sort(list.begin(), list.end(), compareMyDataFunctor());

    return list;
}*/

bool Location::cacheElement(LocationRef& cache, const Location& element, const Location& loc) const {
    if (cache.location) return false;
    if (loc.equals(element)) return false;

    cache.location = &element;
    cache.ref = &loc;
    cache.distance = distance_fast(loc.row, loc.col, element.row, element.col);

    return true;
}

void onTileCallback(const Location& loc, CallbackLocData data)
{
    const Location& senderLocation = *data.senderLocation;

    char locType = LocType(loc);

    if (locType == '*') {
        LocationCache& cache = LocCache(senderLocation);

        const uint8 _FOOD_TYPES_COUNT = 2;
        const uint8 _FOOD_TYPE[2] = {ANY, BLURRED};

        for (int i = 0; i < _FOOD_TYPES_COUNT; i++) {
            uint8& size = cache.nearestFoodSize[_FOOD_TYPE[i]];
            if (size < MAX_CACHE_DEPTH && senderLocation.cacheElement(cache.nearestFood[_FOOD_TYPE[i]][size], loc, senderLocation)) {
                size++;
            }
        }
    } else
    if (locType == '^') {
        LocationCache& cache = LocCache(senderLocation);

        const uint8 _FOOD_TYPES_COUNT = 2;
        const uint8 _FOOD_TYPE[2] = {ANY, FOCUSED};

        for (int i = 0; i < _FOOD_TYPES_COUNT; i++) {
            uint8& size = cache.nearestFoodSize[_FOOD_TYPE[i]];
            if (size < MAX_CACHE_DEPTH && senderLocation.cacheElement(cache.nearestFood[_FOOD_TYPE[i]][size], loc, senderLocation)) {
                size++;
            }
        }
    } else
    if (locType == 'b') {
        LocationCache& cache = LocCache(senderLocation);

        const uint8 _ANT_TYPES_COUNT = 2;
        const uint8 _ANT_TYPE[2] = {ANY, ENEMY};

        for (int i = 0; i < _ANT_TYPES_COUNT; i++) {
            uint8& size = cache.nearestFoodSize[_ANT_TYPE[i]];
            if (size < MAX_CACHE_DEPTH && senderLocation.cacheElement(cache.nearestAnt[_ANT_TYPE[i]][size], loc, senderLocation)) {
                size++;
            }
        }
    } else
    if (locType == 'a') {
        LocationCache& cache = LocCache(senderLocation);

        const uint8 _ANT_TYPES_COUNT = 2;
        const uint8 _ANT_TYPE[2] = {ANY, FRIEND};

        for (int i = 0; i < _ANT_TYPES_COUNT; i++) {
            uint8& size = cache.nearestFoodSize[_ANT_TYPE[i]];
            if (size < MAX_CACHE_DEPTH && senderLocation.cacheElement(cache.nearestAnt[_ANT_TYPE[i]][size], loc, senderLocation)) {
                size++;
            }
        }
    }
}

void Location::cleanCache() const {
    const Location& me = *this;
    LocationCache& cache = LocCache(me);

    memset(&cache, 0, sizeof(LocationCache));

    for (int i = 0; i < ANT_TYPES_COUNT; i++) {
        for (int j = 0; j < MAX_CACHE_DEPTH; j++) {
            cache.nearestAnt[i][j].distance = 255;
        }
    }

    for (int i = 0; i < FOOD_TYPES_COUNT; i++) {
        for (int j = 0; j < MAX_CACHE_DEPTH; j++) {
            cache.nearestFood[i][j].distance = 255;
        }
    }
}

#ifdef __ASSERT
void Location::validateCache(std::string type, const Location* cacheLoc, const Location& foundLoc) const {
    if (cacheLoc) {
        if (!cacheLoc->equals(foundLoc)) {
            logger.logError("cacheLoc != foundLoc");
            logger.debugLog << "DEBUG: " << type << std::endl;
            logger.debugLog << "DEBUG: cacheLoc: " << LocationToString(*cacheLoc) << std::endl;
            logger.debugLog << "DEBUG: foundLoc: " << LocationToString(foundLoc) << std::endl;
        }
    }
}
#endif

LocationCache& Location::updateCache() const
{
    const Location& me = *this;
    LocationCache& cache = LocCache(me);
    if (cache.turn == state.turn) return cache;

    cache.turn = state.turn;
    cleanCache();

    #ifdef __DEBUG
    profiler.beginThinkTime(TT_MAP_SEARCH);
    #endif

    gameMap.callbackArea(*this, state.viewradius, onTileCallback, this);

    #ifdef __DEBUG
    profiler.endThinkTime(TT_MAP_SEARCH);
    #endif

    #ifdef __ASSERT
    validateCache("ANY ANT", cache.nearestAnt[ANY][FIRST].location, this->nearestAnt(ANT_ISFRIEND + ANT_ISENEMY, *this));
    validateCache("FRIEND ANT", cache.nearestAnt[FRIEND][FIRST].location, this->nearestAnt(ANT_ISFRIEND, *this));
    validateCache("ENEMY ANT", cache.nearestAnt[ENEMY][FIRST].location, this->nearestAnt(ANT_ISENEMY));
    //validateCache("ANY FOOD", cache.nearestFood[ANY][FIRST].loc, this->nearestFood(???));
    validateCache("FOCUSED FOOD", cache.nearestFood[FOCUSED][FIRST].location, this->nearestFood(true));
    validateCache("BLURRED FOOD", cache.nearestFood[BLURRED][FIRST].location, this->nearestFood(false));
    #endif

    return cache;
}

const Location& Location::nearestFood(bool focusedFood /*= false*/) const
{
    MapSearch mapSearch = gameMap.find(*this, 16, ((focusedFood) ? FOOD_FOCUSED : FOOD_BLURRED));
    return *mapSearch.location;
}

vector2f Location::getForce(Ant* forAnt, bool attraction /* = true*/, bool repulsion /* = true*/) const
{
    vector2f velocity(0,0);

    #ifdef __DEBUG
    profiler.beginThinkTime(TT_FORCES);
    #endif

    double expandForce = bot.getExpandForce();

    Location relLoc;
    vector2f n;
    double force;

    MagnetData magnetData;
    magnetData.power = 0;

    for (int magnetId = 0; magnetId < state.magnetsCount; magnetId++)
    {
        const Location* magnetLocation = state.magnets[magnetId];
        if (Row(this) == Row(magnetLocation) && Col(this) == Col(magnetLocation)) continue;

        magnetData.distance = gameMap.distance(Row(magnetLocation), Col(magnetLocation), Row(this), Col(this));

        char& magnetType = state.grid[magnetLocation->row][magnetLocation->col];
        applyMagnetData(magnetData, magnetType, *magnetLocation, forAnt);

        if (magnetData.power != 0) {
            relLoc = Location::relativeLocationTo(*magnetLocation);
            n = -vector2f(relLoc.col, relLoc.row).normalized();

            if (magnetData.ignoreDistance) {
                force = magnetData.power;
            } else {
                force = (magnetData.power / (4 * 3.14 * sqr(magnetData.distance / magnetData.div)));
            }

            n *= force;
            if (state.grid[row][col] == 'a') n *= expandForce;

            velocity += n;
        }

    }

    if (velocity.length() > 5) {
        velocity.normalize();
        velocity *= 5;
    }


    #ifdef __DEBUG
    profiler.endThinkTime(TT_FORCES);
    #endif


    return velocity;


    for(int row=0; row<state.rows; row++)
    for(int col=0; col<state.cols; col++)
    {
        if (LocIsFog(row, col)) continue;
        if (LocIsEmpty(row, col)) continue;

        const Location& magnetLocation = Loc(row, col);



        //if (magnetLocation.isType(EMPTY) || magnetLocation.isType(FOG)) continue;
        if ((*this).row == magnetLocation.row && (*this).col == magnetLocation.col) continue;

        const Location* forceTowards = &Loc(row, col);

        char& magnetType = state.grid[row][col];
        bool magnetic = false;
        bool ignoreDistance = false;
        double distance = gameMap.distance(magnetLocation.row, magnetLocation.col, (*this).row, (*this).col);
        double magnetPower = 0;
        double extraDistance = 0.0f;
        double distanceDivision = 5.0f;

        //if(state.grid[row][col] == 'a' && (!forAnt || forAnt != gameMap.getAntAt(magnetLocation))) {
        if (magnetType == 'a') {
            magnetic = true;
            magnetPower = 5;
            distanceDivision = 15;
/*
            if (bot.hasAggressiveMode()) {
                if (distance == -1)
                    distance = gameMap.distance(magnetLocation.row, magnetLocation.col, (*this).row, (*this).col);

                if (magnetLocation.damageArea().enemy >= 1 && distanceToNearestFood > state.viewradius && distance < (state.viewradius + ((magnetLocation.damageArea().enemy - 1) * 2))) {
                    forceTowards = magnetLocation.damageArea().enemyAnts.front();
                    magnetic = true;
                    ignoreDistance = true;
                    magnetPower = -100;
                }
            }*/

/*
            if (distance > state.viewradius + 3) {
                magnetic = false;
                if (distanceToNearestAnt > state.viewradius) {
                    if (forAnt && forAnt->isEnemyInRange(state.viewradius)) {
                        magnetic = false;
                    } else {
                        magnetic = true;
                        magnetPower = -magnetPower;
                    }
                }
            }*/
        }

/*
        if (magnetType == 'b') {
            bool attack = false;
            if (bot.hasAggressiveMode()) attack = true;

            if (attack) {
                magnetic = true;
                magnetPower = -5;
                distanceDivision = 50;
            } else {
                if (forAnt) {
                    //bool nearestFriendly = Location::nearestAnt(ANT, *this).isType(ANT_FRIENDLY);

                    if (distanceToNearestAnt > distanceToNearestAntEnemy) {
                        magnetic = true;
                        magnetPower = 1;
                        distanceDivision = 15;
                    }
                }
            }
        }*/

        if (magnetType == '%' && distance <= 4 /* && (!forAnt || (forAnt && nearestAntNearFood != forAnt))*/) {
            magnetic = true;
            magnetPower = 0.5;
            distanceDivision = 5;
        }

        if (magnetType == '*') {

           // OR rather than checking nearest ant, how about attract only if enemydistance to the food is larger than us to the food
        //if(state.grid[row][col] == '*') {
        // todo: make sure forces from both sides aren't the same, priority apples

            magnetic = true;
            magnetPower = -100;
            distanceDivision = 15;

/*
            Ant* nAnt = magnetLocation.nearestAnt(ANT_ISFRIEND + ANT_NOPATH).getAnt();

            if (magnetic && forAnt && nAnt && nAnt != forAnt && !magnetLocation.collisionLine(nAnt->getLocation())) {
                magnetic = false;
            }

            if (magnetic && forAnt) {
                const Location& nearestEnemy = magnetLocation.nearestAnt(ANT_ISENEMY);
                if (nearestEnemy.isValid()) {
                    double enemyDistance = distance_fast(nearestEnemy.row, nearestEnemy.col, magnetLocation.row, magnetLocation.col);
                    double ourDistance = distance_fast(forAnt->getLocation().row, forAnt->getLocation().col, magnetLocation.row, magnetLocation.col);
                    if (enemyDistance < ourDistance && !nearestEnemy.collisionLine(magnetLocation)) {
                        magnetic = false;
                    }
                }
            }

            if (!magnetic && (magnetLocation.damageArea().enemy >= 1)) {
                if (distance == -1)
                    distance = gameMap.distance(magnetLocation.row, magnetLocation.col, (*this).row, (*this).col);

                if (distance < (state.viewradius + ((magnetLocation.damageArea().enemy - 1) * 2))) {
                    Ant* friendAnt = magnetLocation.nearestAnt(ANT).getAnt();
                    if (friendAnt) {
                        magnetic = true;
                        ignoreDistance = true;
                    }
                }
            }

            if (magnetic) {
                if (distance == -1)
                    distance = gameMap.distance(magnetLocation.row, magnetLocation.col, (*this).row, (*this).col);

                if (distance < state.viewradius) {
                    distance = this->costTo(magnetLocation);
                }
            }*/
        }


        if (magnetic) {
            //if (!repulsion && magnetPower > 0) continue;
            //if (!attraction && magnetPower < 0) continue;
            //if (magnetPower > 0) magnetPower *= (((double)(state.rows+state.cols) / 2.0f) / 100.0f); // Expand the force according to map size
            //if (magnetPower == 0) continue;


            distance = distance_fast(forceTowards->row, forceTowards->col, (*this).row, (*this).col) + extraDistance;

            Location relLoc = Location::relativeLocationTo(*forceTowards);
            vector2f n = -vector2f(relLoc.col, relLoc.row).normalized();


            double magnetForce;

            if (ignoreDistance) {
                magnetForce = magnetPower;
            } else {
                magnetForce = (magnetPower/(4*3.14*sqr(distance/distanceDivision)));
            }

            n *= magnetForce;
            if (state.grid[row][col] == 'a') n *= expandForce;

            velocity += n;
        }
    }

    if (velocity.length() > 5) {
        velocity.normalize();
        velocity *= 5;
    }

    #ifdef __DEBUG
    profiler.endThinkTime(TT_FORCES);
    #endif

    return velocity;
}

int Location::countAnts(double radius, bool predict) const
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_NEAREST_ANT);
    #endif

    int ret = 0;

    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAntById(ant_id);
        if (!ant) continue;

        double dist = distance_fast((*ant->loc).row, (*ant->loc).col, (*this).row, (*this).col);

        if (dist <= radius) {
            ret++;
        } else
        if (predict && dist <= (radius+1) && ant->lastThink < state.turn) {
            ret++;
        }
    }

    #ifdef __DEBUG
    profiler.endThinkTime(TT_NEAREST_ANT);
    #endif

    return ret;
}

//TODO: Location::scoreMove

const Location& Location::nearestAnt(int antFlags /* = ANT_NOFLAGS */, const Location& ignoreAnt /* = LOCATION_UNDEFINED */, const double searchRadius /* = 32 */) const
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_NEAREST_ANT);
    #endif

    const Location* ret = locNull;
    int locationType;

    bool findFriend = false;
    bool findEnemy = false;

    if ((ANT & antFlags)) {
        findFriend = findEnemy = true;
    } else {
        if (ANT_ISFRIEND & antFlags) {
            findFriend = true;
        }
        if (ANT_ISENEMY & antFlags) {
            findEnemy = true;
        }
    }

/*
    logger.debugLog << "antFlags = " << antFlags << std::endl;
    logger.debugLog << "findFriend = " << findFriend << std::endl;
    logger.debugLog << "findEnemy = " << findEnemy << std::endl;
*/
    if (findFriend && findEnemy) {
        locationType = ANT;
    } else
    if (findFriend) {
        locationType = ANT_FRIENDLY;
    } else
    if (findEnemy) {
        locationType = ANT_ENEMY;
    } else {
        #ifdef __DEBUG
        profiler.endThinkTime(TT_NEAREST_ANT);
        #endif

        #ifdef __ASSERT
        logger.logError("Location::nearestAnt: Unknown antFlags");
        logger.debugLog << "DEBUG: antFlags = " << antFlags << std::endl;
        #endif

        return *ret;
    }

    //logger.debugLog << "locationType: " << locationType << std::endl;



    Ant* foundAnt;

    MapSearch mapSearch;
    while (!mapSearch.reachedRadius) {
        mapSearch = gameMap.find(*this, searchRadius, (LocationType)locationType, mapSearch);
        if (!mapSearch.found) break;

        if (mapSearch.location->equals(ignoreAnt)) continue;

        if (locationType == ANT_FRIENDLY) {
            foundAnt = mapSearch.location->getAnt();
            if (!foundAnt) {
                #ifdef __ASSERT
                logger.logError("Location::nearestAnt: ANT_FRIENDLY && mapSearch.location->getAnt() == NULL");
                #endif
                continue;
            }
            if (!foundAnt->hasFlags((AntFlags)antFlags)) continue;
        }

        ret = mapSearch.location;
        break;
    }

    #ifdef __DEBUG
    profiler.endThinkTime(TT_NEAREST_ANT);
    #endif

    return *ret;
}


Location Location::relativeLocationTo(const Location& loc2) const
{
    int d1 = (*this).row-loc2.row,
        d2 = (*this).col-loc2.col;

    if (d1 < 0) d1 = -d1;
    if (d2 < 0) d2 = -d2;

    int dr = state.rows-d1,
        dc = state.cols-d2;

    if (d1 < dr) dr = d1;
    if (d2 < dc) dc = d2;

    int sign_row = sign((*this).row-loc2.row);
    int sign_col = sign((*this).col-loc2.col);

    if (state.rows-d1 == dr) sign_row *= -1;
    if (state.cols-d2 == dc) sign_col *= -1;

    Location loc = Location(-dr * sign_row, -dc * sign_col);
    return loc;
}


bool Location::isWall() const
{
    return state.grid[row][col] == '%';
}

bool Location::hasFood() const
{
    return state.grid[row][col] == '*' || state.grid[row][col] == '^';
}

void Location::focusFood() const
{
    if (state.grid[row][col] == '*') state.grid[row][col] = '^';
}

void Location::blurFood() const
{
    if (state.grid[row][col] == '^') state.grid[row][col] = '*';
}

bool Location::isBlocked() const
{
    if (state.grid[row][col] == '%') return true;
    //if (state.grid[row][col] == 'a') return true;
    //if (state.grid[row][col] == '*') return true;
    //if (state.grid[row][col] == '^') return true;

    return false;
}

bool Location::collisionLine(const Location& loc, bool secondSolve /* = false */) const
{
    if (row == loc.row && col == loc.col) return false;

    #ifdef __DEBUG
    profiler.beginThinkTime(TT_ASTAR);
    #endif

    Location relative = (*this).relativeLocationTo(loc);
    vector2f relativeVector = vector2f(relative.col, relative.row);
    relativeVector.normalize();

    vector2f currentPoint(Location::col, Location::row);

    int i = (relativeVector.x > relativeVector.y) ? 0 : 1;
    if (secondSolve) i++;

    const Location* currentLocation = this;
    while (!distance_fast(currentLocation->row, currentLocation->col, loc.row, loc.col) < 1)
    {
        if (i % 2 == 0) {
            currentPoint.x += relativeVector.x;
        } else {
            currentPoint.y += relativeVector.y;
        }
        currentLocation = &LocWrap(round(currentPoint.y), round(currentPoint.x));
        if (currentLocation->isWall()) {
            if (secondSolve) {
                #ifdef __DEBUG
                profiler.endThinkTime(TT_ASTAR);
                #endif
                return true;
            } else {
                #ifdef __DEBUG
                profiler.endThinkTime(TT_ASTAR);
                #endif
                return collisionLine(loc, true);
            }
        }
        i++;
    }

    #ifdef __DEBUG
    profiler.endThinkTime(TT_ASTAR);
    #endif

    return false;
}
