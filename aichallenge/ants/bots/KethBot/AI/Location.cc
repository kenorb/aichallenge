#include "Location.h"
#include "Ant.h"
#include "Bot.h"
#include <algorithm>
#include "globals.h"


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

    if (row < 0) row += state.rows;
    if (col < 0) col += state.cols;

    if (row >= state.rows) row -= state.rows;
    if (col >= state.cols) col -= state.cols;

    #ifdef __DEBUG
    logger.locationsCreated++;
    #endif
}
int Location::getIndex() {
    return LocToIndex(row, col);
}

void Location::normalize()
{
    while (row < 0) row += state.rows;
    while (col < 0) col += state.cols;

    while (row >= state.rows) row -= state.rows;
    while (col >= state.cols) col -= state.cols;
}

bool Location::think()
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

bool Location::isValid()
{
    if (row < 0)
        return false;

    if (col < 0)
        return false;

    return true;
};

bool Location::hasAnt()
{
    return gameMap.getAntAt(*this) != NULL;
}

bool Location::isAround(const Location& loc)
{
    return Location::distanceTo(loc) < 2;
}

bool funcSortSearchLocations(SearchLocation* loc1, SearchLocation* loc2)
{
    return loc1->cost < loc2->cost;
}

SearchLocation::SearchLocation(Location& _loc, int _cost)
{
    loc = Loc(_loc);
    cost = _cost;
    opened = true;
}

Path* Location::findPathTo(const Location& endLocation)
{
    Path* path = new Path();

    path->setSource(Loc(*this));
    path->setTarget(Loc(endLocation));
    path->found = false;

    //logger.debugLog << "findPathFrom " << LocationToString(path->source) << " to " << LocationToString(path->target) << endl;

    if (Location::equals(path->targetLocation()) || Location::isAround(path->targetLocation())) {
        path->found = true;
        return path;
    }

    list<SearchLocation*> opened;

    vector<SearchLocation*> search_grid(Loc(state.rows-1, state.cols-1).getIndex());
    search_grid[Location::getIndex()] = new SearchLocation(*this, 0);
    opened.push_back(search_grid[Location::getIndex()]);

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

            Location currentLocation = Loc(openedSquare->loc.row + y, openedSquare->loc.col + x);
            int index = currentLocation.getIndex();

            if (search_grid[index] != NULL) continue;

            int locationCost = 1;

            search_grid[index] = new SearchLocation(currentLocation, openedSquare->cost + locationCost);
            search_grid[index]->setRef(&Loc(openedSquare->loc));

            if (currentLocation.equals(path->targetLocation())) {
                path->totalCost = openedSquare->cost + locationCost;

                while (!currentLocation.equals(path->sourceLocation())) {
                    path->moves.push(currentLocation);
                    currentLocation = *search_grid[index]->ref;
                    index = currentLocation.getIndex();
                }

                //path->moves.pop();

                path->found = true;

                return path;
            }

            if (currentLocation.isBlocked()) continue;
            opened.push_back(search_grid[currentLocation.getIndex()]);
        }

        //delete &openedSquare;
        path->iterations++;
/*
        if (path->iterations > 32) {
            path->found = false;
            return path;
        }
*/
    }

    return path;
}

double Location::distanceTo(const Location& loc)
{
    return gameMap.distance((*this), loc);
}

double Location::costTo(const Location& loc, bool precise /* = FALSE */)
{
    double ret;
    if (!precise) {
        bool blocking = Location::collisionLine(loc);

        ret = gameMap.distance((*this), loc) - 1;
        if (ret < 0) ret = 0;

        if (blocking) ret *= 2;

        return ceil(ret);
    } else {
        Path* path = Location::findPathTo(loc);
        if (path->found) {
            ret = path->totalCost;
        } else {
            ret = numeric_limits<double>::max();
        }
        delete path;
    }
    return ret;
}

Location& Location::nearestFood()
{
    double distance = numeric_limits<double>::max();
    Location& ret = LOCATION_UNDEFINED;

    for(int _row=0; _row<state.rows; _row++)
    for(int _col=0; _col<state.cols; _col++)
    {
        if(state.grid[_row][_col] == '*') {
            double dist = gameMap.distance(Loc(_row,_col), (*this));
            if (dist < distance) {
                distance = dist;
                ret = Loc(_row,_col);
            }
        }
    }

    return ret;
}

vector2f Location::getForce(Ant* forAnt)
{
    double force_x = 0.0f;
    double force_y = 0.0f;
    double expandForce = bot.getExpandForce();
    Location& nearestFood = Location::nearestFood();
    vector2f velocity(0,0);

    for(int row=0; row<state.rows; row++)
    for(int col=0; col<state.cols; col++)
    {
        Location& magnetLocation = Loc(row, col);

        if (Location::equals(magnetLocation)) continue;

        bool magnetic = false;
        double magnetPower = 0;
        double extraDistance = 0.0f;
        double distanceDivision = 5.0f;

        if(state.grid[row][col] == 'a' && (!forAnt || forAnt != gameMap.getAntAt(magnetLocation))) {
            magnetic = true;
            magnetPower = 0.5;
            distanceDivision = 15;
        }

        if (state.grid[row][col] >= 0x62 && state.grid[row][col] <= 0x68 && bot.hasAggressiveMode() && !Location::collisionLine(magnetLocation)) {
            magnetic = true;
            magnetPower = -5;
            distanceDivision = 50;
        }

        if(state.grid[row][col] == '%') {
            magnetic = true;
            magnetPower = 0.5;
            distanceDivision = 5;
        }

        //if(state.grid[row][col] == '*' && (!forAnt || magnetLocation.nearestAnt() == forAnt)) {
        if(state.grid[row][col] == '*' && (!forAnt || magnetLocation.nearestAnt(true) == forAnt)) {
            // todo: make sure forces from both sides aren't the same, priority apples
        //if(state.grid[row][col] == '*') {
            magnetic = true;
            magnetPower = -10;
            distanceDivision = 15;
        }


        if (magnetic) {
            double distance = gameMap.distance(*this, magnetLocation) + extraDistance;
            Location relLoc = Location::relativeLocationTo(magnetLocation);

            double magnetForce = (magnetPower/(4*pi*sqr(distance/distanceDivision)));

            double add_force_x = magnetForce * -vector2f(relLoc.col, relLoc.row).normalized().x;
            double add_force_y = magnetForce * -vector2f(relLoc.col, relLoc.row).normalized().y;

            if (state.grid[row][col] == 'a') {
                add_force_x *= expandForce;
                add_force_y *= expandForce;
            }

            force_x += add_force_x;
            force_y += add_force_y;
        }

        velocity = vector2f(force_x, force_y);

        if (velocity.length() > 5) {
            velocity.normalize();
            velocity *= 5;
        }
    }
    return velocity;
}



Ant* Location::nearestAnt(bool ignoreTakenAnts /* = false */)
{
    double distance = numeric_limits<double>::max();

    // todo: use floodfill? should be faster and walls should also be checked
/*
	typedef std::pair<int32_t, int32_t> relPair;
	std::vector<relPair> relList;

	relList.push_back(relPair(1, 0));
	relList.push_back(relPair(0, -1));
    relList.push_back(relPair(-1, 0));
	relList.push_back(relPair(0, 1));
	relList.push_back(relPair(1, -1));
	relList.push_back(relPair(1, 1));
	relList.push_back(relPair(-1, -1));
	relList.push_back(relPair(-1, 1));
*/
    Ant* ret = NULL;
    for (int ant_id = 0; ant_id < (int)state.ants.size(); ant_id++) {
        Ant* ant = getAnt(ant_id);
        if (ignoreTakenAnts && ant->hasPath()) continue;
        double dist = gameMap.distance(ant->getLocation(), (*this));
/*
            // TODO: Accurate distance cost using pathing algorithm
            //dist += 3.0f;
            #ifdef __DEBUG
            //logger.debugLog << "[nearestAnt]: Wall between " << (*ant) << " and " << LocationToString(*this) << endl;

            #endif

            //Location betweenLoc = Location(round((ant->getLocation().row + row) / 2),round((ant->getLocation().col + col) / 2));
            if (dist < 3 && ant->getLocation().collisionLine(*this)) {

                dist += 3.0f;
                #ifdef __DEBUG
                //logger.debugLog << "[nearestAnt]: There is wall between at " << LocationToString(betweenLoc) << endl;
                #endif
            }
*/
        if (dist < distance) {
            distance = dist;
            ret = ant;
        }
    }
    return ret;
}

Location Location::relativeLocationTo(const Location& loc2)
{
    Location loc1 = *this;
    int d1 = abs(loc1.row-loc2.row),
        d2 = abs(loc1.col-loc2.col),
        dr = min(d1, state.rows-d1),
        dc = min(d2, state.cols-d2);

    int sign_row = sign(loc1.row-loc2.row);
    int sign_col = sign(loc1.col-loc2.col);

    if (state.rows-d1 == dr) sign_row *= -1;
    if (state.cols-d2 == dc) sign_col *= -1;

    Location loc = Location(-dr * sign_row, -dc * sign_col);
    return loc;
}


bool Location::hasWall()
{
    return state.grid[row][col] == '%';
}

bool Location::hasFood()
{
    return state.grid[row][col] == '*' || state.grid[row][col] == '^';
}

void Location::focusFood()
{
    if (state.grid[row][col] == '*') state.grid[row][col] = '^';
}

void Location::blurFood()
{
    if (state.grid[row][col] == '^') state.grid[row][col] = '*';
}

bool Location::isBlocked()
{
    if (state.grid[row][col] == '%') return true;
    //if (state.grid[row][col] == '*') return true;
    //if (state.grid[row][col] == '^') return true;

    return false;
}

bool Location::collisionLine(const Location& loc, bool secondSolve /* = false */)
{
    if (row == loc.row && col == loc.col) return false;
    Location relative = (*this).relativeLocationTo(loc);
    vector2f relativeVector = vector2f(relative.col, relative.row);
    relativeVector.normalize();

    vector2f currentPoint(Location::col, Location::row);


    //for (int i = 0; i < distance*2; i++) {
    int i = (relativeVector.x > relativeVector.y) ? 0 : 1;
    if (secondSolve) i++;

    Location currentLocation = (*this);
    while (not currentLocation.distanceTo(loc) < 1) {
        if (i % 2 == 0) {
            currentPoint.x += relativeVector.x;
        } else {
            currentPoint.y += relativeVector.y;
        }
        currentLocation = Loc(currentPoint);
        //currentLocation.addDebugLine(".");
        if (currentLocation.hasWall()) {
            if (secondSolve) {
                //currentLocation.addDebugLine("X");
                return true;
            } else {
                return collisionLine(loc, true);
            }
        }
        i++;
    }
    return false;
    // TODO:
}
