#include "Location.h"
#include "Ant.h"
#include "Bot.h"
#include <algorithm>
#include "globals.h"
#include "Optimizer.h"


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
    return state.grid_damage[row][col];
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
    char& s = state.grid[row][col];
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
    return (loc1->cost) < (loc2->cost);
}

SearchLocation::SearchLocation(const Location& _loc, int _cost)
{
    loc = &Loc(_loc);
    cost = _cost;
    opened = true;
}

Path* Location::findPathTo(const Location& endLocation) const
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_ASTAR);
    #endif

    Path* path = new Path();

    path->setSource(Loc(*this));
    path->setTarget(Loc(endLocation));

    if (Location::isAround(path->targetLocation())) {
        #ifdef __DEBUG
        profiler.endThinkTime(TT_ASTAR);
        #endif
        return path;
    }

    int index = LocToIndex(this->row, this->col);

    list<SearchLocation*> opened;

    vector<SearchLocation*> search_grid(MAX_GRID_INDEX);
    search_grid[index] = new SearchLocation(*this, 0);
    opened.push_back(search_grid[index]);

    while (opened.size() > 0) {
        opened.sort(funcSortSearchLocations);

        SearchLocation* openedSquare = opened.front();
        openedSquare->closeSquare();
        opened.remove(openedSquare);

        for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)
        {
            // Don't let diagonal movement
            if (x != 0 && y != 0) continue;
            if (x == 0 && y == 0) continue;

            const Location* currentLocation = &Loc(openedSquare->getLocation().row + y, openedSquare->getLocation().col + x);
            index = LocToIndex(currentLocation->row, currentLocation->col);

            if (search_grid[index] != NULL) continue;

            int locationCost = 1;

            search_grid[index] = new SearchLocation(*currentLocation, openedSquare->cost + locationCost);
            search_grid[index]->setRef(Loc(openedSquare->getLocation()));

            if (currentLocation->row == path->targetLocation().row && currentLocation->col == path->targetLocation().col) {
                path->totalCost = openedSquare->cost + locationCost;

                while (!currentLocation->equals(path->sourceLocation())) {
                    path->moves.push(*currentLocation);
                    currentLocation = &search_grid[index]->getReference();
                    index = LocToIndex(currentLocation->row, currentLocation->col);
                }

                //path->moves.pop();

                #ifdef __DEBUG
                profiler.endThinkTime(TT_ASTAR);
                #endif

                return path;
            }

            if (currentLocation->isBlocked()) continue;
            opened.push_back(search_grid[LocToIndex(currentLocation->row, currentLocation->col)]);
        }

        //delete &openedSquare;
        //path->iterations++;
/*
        if (path->iterations > 32) {
            delete path;
            return NULL;
        }*/

    }

    delete path;

    #ifdef __DEBUG
    profiler.endThinkTime(TT_ASTAR);
    #endif

    return NULL;
}

double Location::costTo(const Location& loc, bool precise /* = FALSE */) const
{
    double ret;
    precise = false;
/*
    #ifndef __DEBUG
    if (state.timeLeft() >= 500) precise = true;
    #endif
*/
    if (!precise) {
        bool blocking = Location::collisionLine(loc);

        ret = distance_fast((*this).row, (*this).col, loc.row, loc.col) - 1;
        if (ret < 0) ret = 0;

        if (blocking) ret *= 2.5;

        return ceil(ret);
    } else {
        Path* path = Location::findPathTo(loc);
        if (path) {
            ret = path->totalCost;
        } else {
            ret = numeric_limits<double>::max();
        }
        delete path;
    }
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


const Location& Location::nearestFood(bool focusedFood /*= false*/) const
{
    MapSearch mapSearch = gameMap.find(*this, 16, ((focusedFood) ? FOOD_FOCUSED : FOOD_BLURRED));
    return *mapSearch.location;
}

vector2f Location::getForce(Ant* forAnt, bool attraction /* = true*/, bool repulsion /* = true*/) const
// todo: use flags
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_FORCES);
    #endif

    double expandForce = bot.getExpandForce();
    const Location& nearestFood = Location::nearestFood();

    double distanceToNearestFood = MAX_DISTANCE;

    if (nearestFood.isValid()) {
        distanceToNearestFood = distance_fast(row, col, nearestFood.row, nearestFood.col);
    }

    Ant* nearestAntNearFood  = NULL;
    if (forAnt) {
        //if (nearestFood.isValid()) nearestAntNearFood = nearestFood.nearestAnt(true); // 2
        nearestAntNearFood = nearestFood.nearestAnt(ANT_ISFRIEND).getAnt(); // 3
    }
    vector2f velocity(0,0);

    for(int row=0; row<state.rows; row++)
    for(int col=0; col<state.cols; col++)
    {
        const Location& magnetLocation = Loc(row, col);

        if ((*this).row == magnetLocation.row && (*this).col == magnetLocation.col) continue;

        bool magnetic = false;
        bool ignoreDistance = false;
        double distance = -1;
        double magnetPower = 0;
        double extraDistance = 0.0f;
        double distanceDivision = 5.0f;


        if(state.grid[row][col] == 'a' && (!forAnt || forAnt != gameMap.getAntAt(magnetLocation))) {
            magnetic = true;
            magnetPower = 1;
            distanceDivision = 15;
        }

/*
        if(state.grid[row][col] == 'a' && (!forAnt || forAnt != gameMap.getAntAt(magnetLocation))) {
            ignoreDistance = true;
            magnetic = true;
            magnetPower = 5;
            distanceDivision = 15;

            distance = gameMap.distance(magnetLocation.row, magnetLocation.col, (*this).row, (*this).col);
            if (distance > state.viewradius) magnetic = false;
        }
*/

        if (state.grid[row][col] == 'b') {
            if (bot.hasAggressiveMode()) {
                magnetic = true;
                magnetPower = -5;
                distanceDivision = 50;
            } else {
                if (forAnt) {
                    bool nearestFriendly = Location::nearestAnt(ANT, *this).isType(ANT_FRIENDLY);

                    if (!nearestFriendly) {
                        magnetic = true;
                        magnetPower = 1;
                        distanceDivision = 15;
                    }
                }
            }
        }

        if(state.grid[row][col] == '%' && (!forAnt || (forAnt && nearestAntNearFood != forAnt))) {
            magnetic = true;
            magnetPower = 0.5;
            distanceDivision = 5;

            if (distance == -1)
                distance = gameMap.distance(magnetLocation.row, magnetLocation.col, (*this).row, (*this).col);

            if (distance > 4) magnetic = false;
        }

        if (state.grid[row][col] == '*') {

           // OR rather than checking nearest ant, how about attract only if enemydistance to the food is larger than us to the food
        //if(state.grid[row][col] == '*') {
        // todo: make sure forces from both sides aren't the same, priority apples

            magnetic = true;
            magnetPower = -100;
            distanceDivision = 15;


            if (magnetic && forAnt && magnetLocation.nearestAnt(ANT_ISFRIEND + ANT_NOPATH).getAnt() != forAnt) {
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

            if (!magnetic) {

                if (distance == -1)
                    distance = gameMap.distance(magnetLocation.row, magnetLocation.col, (*this).row, (*this).col);

                if (distance < state.viewradius) {
                    Ant* friendAnt = magnetLocation.nearestAnt(ANT).getAnt();
                    if (friendAnt && magnetLocation.damageArea().enemy >= 1) {
                        magnetic = true;
                        ignoreDistance = true;
                    }
                }
            }
        }


        if (magnetic) {
            if (!repulsion && magnetPower > 0) continue;
            if (!attraction && magnetPower < 0) continue;
            if (magnetPower > 0) magnetPower *= (((double)(state.rows+state.cols) / 2.0f) / 100.0f); // Expand the force according to map size
            if (magnetPower == 0) continue;

            if (distance == -1)
                distance = distance_fast(magnetLocation.row, magnetLocation.col, (*this).row, (*this).col) + extraDistance;

            Location relLoc = Location::relativeLocationTo(magnetLocation);
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

const Location& Location::nearestAnt(int antFlags /* = ANT_NOFLAGS */, const Location& ignoreAnt /* = LOCATION_UNDEFINED */) const
{
    #ifdef __DEBUG
    profiler.beginThinkTime(TT_NEAREST_ANT);
    #endif

    const Location* ret = locNull;
    int locationType;

    bool findFriend, findEnemy;

    if ((ANT & antFlags) == ANT) {
        findFriend = findEnemy = true;
    } else {
        findFriend = (ANT_ISFRIEND & antFlags) == ANT_ISFRIEND;
        findEnemy = (ANT_ISENEMY & antFlags) == ANT_ISENEMY;
    }

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

    Ant* foundAnt;

    MapSearch mapSearch;
    while (!mapSearch.reachedRadius) {
        mapSearch = gameMap.find(*this, 32, (LocationType)locationType, mapSearch);
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


    //for (int i = 0; i < distance*2; i++) {
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
        currentLocation = &Loc(currentPoint);
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
    // TODO:
}
