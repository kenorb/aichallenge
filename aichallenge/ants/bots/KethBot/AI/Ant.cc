#include "Ant.h"

#ifdef __DEBUG
#include "Logger.h"
#endif

#include "Location.h"
#include "const.h"
#include "Bot.h"
#include "Map.h"

// STRATEGY:
// Calculate accurate percent of *discovered map*
// Calculate accurate percent of *area covered*
// Calculate for each ant average *push force* against all the other ants and walls
// Push each ant by *invidual force* according to the *discovered map* in order to keep the *50 percent* balance

int totalAnts = 0;

Ant::Ant(State &_state, Location &_loc)
{
    id = totalAnts;
    totalAnts++;

    path = NULL;
    timeAlive = 0;
    lastThink = 0;
    movePriority = 0;

    partner = NULL;


    loc = &Loc(_loc);

    //loc.col = _loc.col;
    //loc.row = _loc.row;

    state.structuralAnts.push_back(this);

    physicalPosition((double)loc->col, (double)loc->row);
    velocity(0,0);

    #ifdef __DEBUG
    logger.debugLog << "EVENT: New " << (*this) << " at " << LocationToString(getLocation()) << endl;
    #endif

    Ant::updatePriority();
}

Ant::~Ant()
{
    #ifdef __DEBUG
    logger.debugLog << "EVENT: " << (*this) << " dies at " << LocationToString(getLocation()) << endl;
    #endif

    if (hasPath()) {
        path->targetLocation().blurFood();
    }

    state.structuralAnts.remove(this);
}

void Ant::updatePriority()
{
    movePriority = gameMap.distance(Ant::getLocation(), bot.startLocation);
}

void Ant::onMove(Location& toLoc)
{
    #ifdef __DEBUG
    logger.debugLog << "EVENT: " << (*this) << " moves from " << LocationToString(getLocation()) << " to " << LocationToString(toLoc) << endl;
    #endif

    loc = &Loc(toLoc);

}

bool Ant::canBePlacedAt(Location& loc)
{
    if (state.grid[loc.row][loc.col] == '%') return false;
    if (state.grid[loc.row][loc.col] == 'a') return false;
    if (state.grid[loc.row][loc.col] == '*') return false; // Server is blocking? Ants die when moved inside food?
    if (state.grid[loc.row][loc.col] == '^') return false; // Server is blocking? Ants die when moved inside food?
    if (gameMap.getAntAt(loc)) return false;

    // maximum radius from nearest ant

    return true;
}

void Ant::onThink()
{
    if (lastThink == state.turn) {
        #ifdef __DEBUG
        logger.debugLog << "Ant trying to execute onThink() again" << endl;
        #endif
        return;
    }
    lastThink = state.turn;

    int nextMove = getNextMove();

    #ifdef __DEBUG
    bot.addChecksumValue(nextMove);
    #endif

    if (nextMove != NO_MOVE) state.makeMove(getLocation(), nextMove);

    timeAlive++;
}

bool Ant::hasEnoughForceToMove()
{
    double velocity = gameMap.distance_vector(Ant::getLocation(), physicalPosition);
    return velocity > 0;
}

void Ant::deletePath()
{
    delete Ant::path;
    Ant::path = NULL;
}

void Ant::setPathTo(Location& target)
{
    #ifdef __DEBUG
    logger.debugLog << "Path from " << LocationToString(getLocation()) << " to " << LocationToString(target) << endl;
    #endif
    Ant::path = Ant::getLocation().findPathTo(target);
    if (!path->found || path->moves.size() == 0) {
        #ifdef __DEBUG
        logger.logWarning("Path not found!");
        #endif
        path = NULL;
    } else
    if (path->found) {
        //path->moves.bottom();
    }
}

int Ant::pathStepsLeft() {
    if (!path) return numeric_limits<int>::max();
    return path->moves.size();
}

void Ant::prepareMove()
{
    //logger.debugLog << "Ant::prepareMove(): " << (*this) << endl;
    double force_x = 0.0f;
    double force_y = 0.0f;

    double expandForce = bot.getExpandForce();


    Location& nearestFood = Ant::getLocation().nearestFood();

    bool cancelPath = false;

    // todo
    //  - dont go too far from nearest ant (vision range + 1 maybe)

    // todo:
    //  - if no food
    //  - glue on to the wall
    //  - walk until food is around
    //  - make sure not to walk too far somehow and not to lose advantage
    //  - only glue if less than 60% coverage

    // todo:
    // low amount of ants = avoid dying
    // (or maybe if i'm starting to loss (by checking income per last 1/3 f the game), then avoid attacking)

    // new func validate path
    if (path && path->moves.size() == 0) cancelPath = true; else
    if (path && !path->targetLocation().hasFood()) cancelPath = true; else
    if (path && !nearestFood.equals(path->targetLocation()) && Ant::getLocation().costTo(nearestFood) < Ant::pathStepsLeft() && !Ant::getLocation().collisionLine(nearestFood) && nearestFood.nearestAnt() == this) {
        #ifdef __DEBUG
        logger.debugLog << "Canceled path food for " << (*this) << ", found closer new path without wall to food at " << LocationToString(nearestFood) << endl;
        #endif

        cancelPath = true;
    } else
    if (path) {
        Ant* antNearFood = path->targetLocation().nearestAnt(true);

        if (antNearFood && !antNearFood->getLocation().collisionLine(path->targetLocation())) {
            if (antNearFood != this && antNearFood->getLocation().costTo(path->targetLocation()) < Ant::pathStepsLeft()) {
                #ifdef __DEBUG
                logger.debugLog << "Canceled path food for " << (*this) << ", found closer new path without wall to food at " << LocationToString(nearestFood) << endl;
                #endif

                cancelPath = true;
            }
        }
    }

    if (cancelPath) {
        path->targetLocation().blurFood();
        Ant::deletePath();
    }

    // new func update path
    if (!Ant::hasPath() && nearestFood.isValid() && nearestFood.nearestAnt(true) == this && Ant::getLocation().collisionLine(nearestFood)) {
    //if (!Ant::hasPath() && nearestFood.isValid() && nearestFood.nearestAnt() == this) {

        if (Ant::getLocation().distanceTo(nearestFood) >= 2) {
            #ifdef __DEBUG
            logger.debugLog << "New food for " << (*this) << " through a wall at " << LocationToString(nearestFood) << endl;
            #endif

            Ant::setPathTo(nearestFood);
            nearestFood.focusFood();
        }
    }

    if (!Ant::hasPath()) {
        velocity = Ant::getLocation().getForce(this);
    }

/*
    std::list<Location*>::iterator iter_ant;
    for (iter_ant = gameMap.getEnemyAnts().begin(); iter_ant != gameMap.getEnemyAnts().end(); iter_ant++)
    {
        Location* enemyAnt = (*iter_ant);
    }*/

    /*
        if (enemy closer than 3 squares) {
            try to do formation towards the enemy OR step away if opponent has better formation
        }
    */

    Ant::updatePriority();
}

int Ant::getNextMove()
{
    #ifdef __DEBUG
    logger.debugLog << *this << endl;
    #endif

    int ret = NO_MOVE;

    if (Ant::hasPath() && path->moves.size() > 0) {
        Location& locationTo = path->moves.top();
        Location relLoc = Ant::getLocation().relativeLocationTo(locationTo);

        #ifdef __DEBUG
        logger.debugLog << *this << "I have path, next location is " << LocationToString(locationTo) << ", canBePlaced: " << Ant::canBePlacedAt(locationTo) << endl;
        logger.debugLog << state.grid[locationTo.row][locationTo.col] << endl;
        #endif

        locationTo.think();

        if (!Ant::canBePlacedAt(locationTo)) {
            Ant::deletePath();
            Ant::prepareMove();
            return Ant::getNextMove();
        }

        velocity = vector2f(relLoc.col, relLoc.row);
        path->moves.pop();

    }

    physicalPosition = vector2f(getLocation().col + velocity.x, getLocation().row + velocity.y);

    double dist = numeric_limits<double>::max();
    if (Ant::hasEnoughForceToMove())
    {
        // Move into some direction
        for(int dir=0; dir<4; dir++)
        {
            Location locationTo = state.getLocation(Ant::getLocation(), dir);
            locationTo.think();

            if (Ant::canBePlacedAt(locationTo))
            {
                double distanceCost = gameMap.distance_vector(locationTo, physicalPosition);
                if (distanceCost < dist) {
                    ret = dir;
                    dist = distanceCost;
                }
            }
        }
    }

    // Final solver
    if (ret != NO_MOVE)
    {
        Location locationTo = state.getLocation(Ant::getLocation(), ret);
        if (!Ant::canBePlacedAt(locationTo)) return getNextMove();
    }

    if (ret != NO_MOVE) hasMoved = true;
    return ret;
}


ostream& operator<<(ostream &out, const Ant& ant)
{
	out << "[ANT #" << (int)ant.id << "]";
	return out;
}

