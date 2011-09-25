#ifdef __DEBUG
#include "Logger.h"
#endif
#include "Optimizer.h"

#include "const.h"
#include "Bot.h"
#include "Map.h"
#include "enums.h"

#include <algorithm>
#include <functional>
#include <string.h>

// STRATEGY:
// Calculate accurate percent of *discovered map*
// Calculate accurate percent of *area covered*
// Calculate for each ant average *push force* against all the other ants and walls
// Push each ant by *invidual force* according to the *discovered map* in order to keep the *50 percent* balance

int totalAnts = 0;

Ant::Ant(State &_state, const Location &_loc)
{
    id = totalAnts;
    totalAnts++;

    path = NULL;
    seenEnemy = false;
    timeAlive = 0;
    lastThink = 0;
    predictionMove = MOVE_UNKNOWN;
    forceNextMove = MOVE_UNKNOWN;

    partner = NULL;

    loc = &_loc;

    state.structuralAnts.push_back(this);

    physicalPosition((double)loc->col, (double)loc->row);
    velocity(0,0);

    #ifdef __DEBUG
    logger.debugLog << "EVENT: New " << (*this) << " at " << LocationToString(getLocation()) << endl;
    #endif
}

Ant::~Ant()
{
    #ifdef __DEBUG
    logger.debugLog << "EVENT: " << (*this) << " dies at " << LocationToString(getLocation()) << endl;
    #endif

    if (path && path->hasTarget()) {
        #ifdef __DEBUG
        logger.debugLog << *this << " died. Blurring food at " << LocationToString(path->targetLocation()) << endl;
        #endif
        path->targetLocation().blurFood();

    }

    state.structuralAnts.remove(this);
}

AntFlags Ant::getFlags() {
    int ret = 0;
    if (Ant::path) ret &= ANT_HASPATH; else ret &= ANT_NOPATH;
    if (AntLoc(this).damageArea().enemy >= 1) ret &= ANT_UNDERATTACK;
    if (Ant::seenEnemy) ret &= ANT_SEENENEMY;
    if (Ant::hasMoved) ret &= ANT_DIDMOVE;
    if (Ant::lastThink >= state.turn) ret &= ANT_DIDTHINK;

    return (AntFlags)ret;
}

bool Ant::hasFlags(AntFlags flag)
{
    //if ((flag & ANT_ISFRIEND) == flag) return true;
    if ((ANT_HASPATH & flag) == ANT_HASPATH && !path) return false;
    if ((ANT_NOPATH & flag) == ANT_NOPATH && path) return false;
    if ((ANT_UNDERATTACK & flag) == ANT_UNDERATTACK && AntLoc(this).damageArea().enemy == 0) return false;
    if ((ANT_SEENENEMY & flag) == ANT_SEENENEMY&& !seenEnemy) return false;
    if ((ANT_DIDMOVE & flag) == ANT_DIDMOVE && !hasMoved) return false;
    if ((ANT_DIDTHINK & flag) == ANT_DIDTHINK && Ant::lastThink < state.turn) return false;

    return true;
}

void Ant::onNewTurn()
{
    moveIterations = 0;
    forceNextMove = MOVE_UNKNOWN;
}

void Ant::onMove(const Location& toLoc)
{
    #ifdef __DEBUG
    logger.debugLog << "EVENT TURN # " << state.turn << ": " << (*this) << " (" << (path ? "with path" : "no path") << ") moves from " << LocationToString(getLocation()) << " to " << LocationToString(toLoc) << endl;
    #endif

    loc = &toLoc;
}

bool Ant::canBePlacedAt(const Location& _loc)
{
    if (state.grid[_loc.row][_loc.col] == 'a' && _loc.getAnt() == this) return true;

    switch (state.grid[_loc.row][_loc.col]) {
        case '%': return false;
        case 'a': return false;
        case '*': return false;
        case '^': return false;
    }

    // if (gameMap.getAntAt(_loc) && _loc.getAnt() != this) return false;
    // this solves bugs if we make a move before ant thinking

    return true;
}

void Ant::onThink()
{
    #ifdef __DEBUG
    codeDepth++;
    logger.debugLog << "Ant::onThink(): " << (*this) << endl;
    #endif

    if (lastThink == state.turn) {
        #ifdef __DEBUG
        logger.debugLog << "Ant trying to execute onThink() again" << endl;
        codeDepth--;
        #endif
        return;
    }

    lastThink = state.turn;
    predictionMove = MOVE_UNKNOWN;

    int nextMove = getNextMove();

    #ifdef __DEBUG
    bot.addChecksumValue(nextMove);
    state.moves << (int)(nextMove + 2);
    #endif

    if (nextMove != NO_MOVE && nextMove != MOVE_DANAGEROUS) state.makeMove(getLocation(), nextMove);

    timeAlive++;

    #ifdef __DEBUG
    codeDepth--;
    #endif
}

bool Ant::hasEnoughForceToMove()
{
    double velocity = gameMap.distance_vector(AntLoc(this), physicalPosition);
    return velocity > 0;
}

void Ant::deletePath()
{
    if (path) gameMap.pathPool->Release(Ant::path);
    path = NULL;
}

void Ant::setPathTo(const Location& target)
{
    Ant::path = AntLoc(this).findPathTo(target);
    #ifdef __DEBUG
    logger.debugLog << "Path from " << LocationToString(getLocation()) << " to " << LocationToString(target) << " (" << path->stepsLeft() << " steps)" <<  endl;
    #endif
    if (!path) {
        #ifdef __DEBUG
        logger.debugLog << "Path not found!" <<  endl;
        #endif
        return;
    }
}

bool Ant::needsCancelPath()
{
    if (!path) return false;

    if (AntLoc(this).isAround(path->targetLocation())) {
        return true;
    }

    return false;
}

void Ant::prepareMove()
{
    #ifdef __DEBUG
    codeDepth++;
    profiler.beginThinkTime("Ant::prepareMove()");
    logger.debugLog << "Ant::prepareMove(): " << (*this) << endl;
    #endif

    int i = 0;

    while (true) {
        if (i >= MAX_CACHE_DEPTH) break;

        // todo: use food priority to pick the best target
        LocationRef& foodAny = AntFindFood(this, ANY, i);
        if (!didFind(foodAny)) break;

        const Location& foodLocation = *foodAny.location;

        if (LocFindAnt(foodLocation, FRIEND, FIRST).location->equals(AntLoc(this))) {
            forceNextMove = gameMap.stepTowards(AntLoc(this), *foodAny.location);
            break;
        }

        i++;
    }

    #ifdef __ASSERT
    LocationRef& foodAny = AntFindFood(this, ANY, FIRST);
    LocationRef& foodBlurred = LocCache(AntLoc(this)).nearestFood[BLURRED][FIRST];
    if ((didFind(foodAny) && !didFind(foodBlurred)) || (didFind(foodBlurred) && !didFind(foodAny))) {
        logger.logError("foodAny exists && foodBlurred does not");
    }
    #endif

    if (forceNextMove == MOVE_UNKNOWN) {
        velocity = AntLoc(this).getForce(this);
        physicalPosition = vector2f(getLocation().col + velocity.x, getLocation().row + velocity.y);
    }

    #ifdef __DEBUG
    profiler.endThinkTime("Ant::prepareMove()");
    codeDepth--;
    #endif
}

bool Ant::isEnemyInRange(const double radius) {
    LocationRef& enemy = LocFindAnt(AntLoc(this), ENEMY, FIRST);
    if (didFind(enemy) && enemy.distance <= radius) return true;

    const Location& nearestAnt = getLocation().nearestAnt(ANT_ISENEMY, *locNull, radius);
    if (nearestAnt.isValid()) {
        return nearestAnt.isType(ANT_ENEMY);
    }
    return false;
}

int Ant::scoreMove(directionMove& move)
{
    const Location& locationTo = state.getLocation(AntLoc(this), move.direction);
    // todo: score moves accordingly to damage grid
    return 0;
}

int Ant::getNextMove(bool solveCollision /* = true */)
{
    #ifdef __DEBUG
    codeDepth++;
    profiler.beginThinkTime("Ant::getNextMove()");
    #endif

    #ifdef __DEBUG
    logger.debugLog << "Ant::getNextMove(): " << (*this) << endl;
    #endif

    moveIterations++;

    int ret = NO_MOVE;

    if (forceNextMove != MOVE_UNKNOWN) {
        Location relLoc = AntLoc(this).relativeLocationTo(state.getLocation(AntLoc(this), forceNextMove));
        velocity = vector2f(relLoc.col, relLoc.row);
    }

    physicalPosition = vector2f(getLocation().col + velocity.x, getLocation().row + velocity.y);

    if (Ant::hasEnoughForceToMove())
    {
        std::vector<directionMove> moves;

        for(int dir = -1; dir < 4; dir++)
        {
            const Location& locationTo = state.getLocation(AntLoc(this), dir);
            if (LocIsWall(locationTo.row, locationTo.col)) continue;

            directionMove move;
            move.direction = dir;

            if (dir == NO_MOVE) {
                move.cost = 100;
            } else {
                move.cost = gameMap.distance_vector(locationTo, physicalPosition);
                move.cost -= Ant::scoreMove(move);

                if (gameMap.isDeadEnd(AntLoc(this), dir)) move.cost *= 10;
            }

            moves.push_back(move);
        }

        if (moves.size() >= 1) {
            std::sort(moves.begin(), moves.end(), directionMove::sortMoves());

            for (vector<directionMove>::iterator move = moves.begin(); move != moves.end(); ++move)
            {
                predictionMove = (*move).direction;
                const Location& locationTo = state.getLocation(AntLoc(this), predictionMove);
                locationTo.think();

                if (Ant::canBePlacedAt(locationTo)) {
                    ret = (*move).direction;
                    break;
                }
            }
        }
    }

    const Location& locationTo = state.getLocation(AntLoc(this), ret);

    // Final solver
    if (ret != NO_MOVE && ret != MOVE_DANAGEROUS)
    {
        if (solveCollision && !Ant::canBePlacedAt(locationTo)) {
            #ifdef __DEBUG
            profiler.endThinkTime("Ant::getNextMove()");
            codeDepth--;
            #endif
            return getNextMove(false);
        }
    }

    if (AntFindFood(this, ANY, FIRST).distance <= state.spawnradius) {
        ret = NO_MOVE;
    }

    if (ret != NO_MOVE && ret != MOVE_DANAGEROUS) hasMoved = true;

    #ifdef __DEBUG
    profiler.endThinkTime("Ant::getNextMove()");
    codeDepth--;
    #endif

    return ret;
}


ostream& operator<<(ostream &out, const Ant& ant)
{
    #ifdef __DEBUG
	out << "[ANT #" << (int)ant.id << "] " << LocationToString(*ant.loc);
	#endif
	return out;
}
