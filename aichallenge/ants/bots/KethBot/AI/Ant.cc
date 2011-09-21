#ifdef __DEBUG
#include "Logger.h"
#endif
#include "Optimizer.h"

#include "const.h"
#include "Bot.h"
#include "Map.h"

#include <algorithm>
#include <functional>

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
    movePriority = 0;
    predictionMove = MOVE_UNKNOWN;

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
    if (Ant::getLocation().damageArea().enemy >= 1) ret &= ANT_UNDERATTACK;
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
    if ((ANT_UNDERATTACK & flag) == ANT_UNDERATTACK && Ant::getLocation().damageArea().enemy == 0) return false;
    if ((ANT_SEENENEMY & flag) == ANT_SEENENEMY&& !seenEnemy) return false;
    if ((ANT_DIDMOVE & flag) == ANT_DIDMOVE && !hasMoved) return false;
    if ((ANT_DIDTHINK & flag) == ANT_DIDTHINK && Ant::lastThink < state.turn) return false;

    return true;
}

void Ant::updatePriority()
{
    movePriority = distance_fast((*loc).row, (*loc).col, bot.startLocation.row, bot.startLocation.col);
}

void Ant::onNewTurn()
{
    moveIterations = 0;
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
    switch (state.grid[_loc.row][_loc.col]) {
        case '%': return false;
        case 'a': return false;
        case '*': return false;
        case '^': return false;
    }

    if (gameMap.getAntAt(_loc) && _loc.getAnt() != this) return false;

    return true;
}

double Ant::distanceTo(const Location& loc2) {
    return distance_fast((*loc).row, (*loc).col, loc2.row, loc2.col);
}

double Ant::distanceTo(Ant* ant) {
    return distance_fast((*ant->loc).row, (*ant->loc).col, (*loc).row, (*loc).col);
};

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
    state.moves << (int)nextMove+2;
    #endif

    if (nextMove != NO_MOVE && nextMove != MOVE_DANAGEROUS) state.makeMove(getLocation(), nextMove);

    timeAlive++;

    #ifdef __DEBUG
    codeDepth--;
    #endif
}

bool Ant::hasEnoughForceToMove()
{
    double velocity = gameMap.distance_vector(Ant::getLocation(), physicalPosition);
    return velocity > 0;
}

void Ant::deletePath()
{
    if (path) delete Ant::path;
    path = NULL;
}

void Ant::setPathTo(const Location& target)
{
    Ant::path = Ant::getLocation().findPathTo(target);
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

void Ant::prepareMove()
{
    #ifdef __DEBUG
    codeDepth++;
    profiler.beginThinkTime("Ant::prepareMove()");
    logger.debugLog << "Ant::prepareMove(): " << (*this) << endl;
    #endif

    const Location& nearestFood = Ant::getLocation().nearestFood();
    double distanceToNearestFood = MAX_DISTANCE;

    if (nearestFood.isValid()) {
        distanceToNearestFood = distance_fast(Ant::getLocation().row, Ant::getLocation().col, nearestFood.row, nearestFood.col);
    }

    bool cancelPath = false;

    // cleanup: new func validate path
    if (path && path->stepsLeft() == 0) cancelPath = true; else
    if (path && !path->targetLocation().hasFood()) cancelPath = true; else
    if (path && nearestFood.isValid() && !nearestFood.equals(path->targetLocation()) && Ant::getLocation().costTo(nearestFood) < Ant::path->stepsLeft()) {
        // todo: use/choose by food priority
        // food priority = attraction of food to other foods on the map
        // extra priority according to further food from start distance
        // extra priotity according to if enemy is further than us
        // magnetpower *= priority
        #ifdef __DEBUG
        logger.debugLog << "Canceled path food for " << (*this) << ", found closer new path with fewer steps at " << LocationToString(nearestFood) << endl;
        #endif

        cancelPath = true;
    } else
    if (path) {
        const Location& antNearFood = path->targetLocation().nearestAnt(ANT_ISFRIEND + ANT_NOPATH);

        if (antNearFood.isValid() && !antNearFood.collisionLine(path->targetLocation())) {
            if (!antNearFood.equals(Ant::getLocation()) && antNearFood.costTo(path->targetLocation()) < Ant::path->stepsLeft()) {
                #ifdef __DEBUG
                logger.debugLog << "Canceled path food for " << (*this) << ", found closer new path without wall to food at " << LocationToString(nearestFood) << endl;
                #endif

                cancelPath = true;
            }
        }
    }

    if (cancelPath) {
        if (path->hasTarget()) path->targetLocation().blurFood();
        Ant::deletePath();
    }

    // new func update path
    if (!Ant::path && nearestFood.isValid() && nearestFood.nearestAnt(ANT_ISFRIEND + ANT_NOPATH).equals(Ant::getLocation()) && !nearestFood.isAround(Ant::getLocation()) && (Ant::getLocation().collisionLine(nearestFood) || distanceToNearestFood >= state.viewradius)) {

    //if (!Ant::hasPath() && nearestFood.isValid() && nearestFood.nearestAnt() == this) {
        #ifdef __DEBUG
        logger.debugLog << "New path to food for " << (*this) << " through a wall at " << LocationToString(nearestFood) << endl;
        #endif

        Ant::setPathTo(nearestFood);

        if (path && path->stepsLeft() >= 1) {

/*
            Location& locationTo = path->moves.top();
            locationTo.think();

            if (path && path->found && path->moves.size() >= 1 && Ant::canBePlacedAt(locationTo)) {
                #ifdef __DEBUG
                logger.debugLog << *this << " Focusing food at " << LocationToString(nearestFood) << endl;
                #endif
*/
            nearestFood.focusFood();
        } else {
            Ant::deletePath();
        }
    }

    if (!Ant::hasPath()) {
        velocity = Ant::getLocation().getForce(this);
    }

    physicalPosition = vector2f(getLocation().col + velocity.x, getLocation().row + velocity.y);
/*
    std::vector<directionMove> moves;
    for(int dir = 0; dir < 4; dir++)
    {
        Location locationTo = state.getLocation(Ant::getLocation(), dir);
        directionMove move;
        move.direction = dir;
        move.cost = gameMap.distance_vector(locationTo, physicalPosition);
        moves.push_back(move);
    }

    std::sort(moves.begin(), moves.end(), directionMove::sortMoves());

    for (vector<directionMove>::iterator move = moves.begin(); move != moves.end(); ++move)
    {
        Location locationTo = state.getLocation(Ant::getLocation(), (*move).direction);

        if (Ant::canBePlacedAt(locationTo)) {
            if (locationTo.damageArea().enemy == 0) {
                predictionMove = (*move).direction;
            }
            break;
        }
    }*/

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

    #ifdef __DEBUG
    profiler.endThinkTime("Ant::prepareMove()");
    codeDepth--;
    #endif
}

bool Ant::isEnemyInRange(const double radius) {
    const Location& nearestAnt = getLocation().nearestAnt(ANT_ISENEMY, *locNull, radius);
    if (nearestAnt.isValid()) {
        return nearestAnt.isType(ANT_ENEMY);
    }
    return false;
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
    //if (moveIterations > 4) return NO_MOVE;

    int ret = NO_MOVE;

    if (path && path->stepsLeft() > 0) {
        Location& locationTo = path->moves.top();

        Location relLoc = Ant::getLocation().relativeLocationTo(locationTo);
        locationTo.think();

        #ifdef __DEBUG
        logger.debugLog << *this << " I have path, next location is " << LocationToString(locationTo) << ", canBePlaced: " << Ant::canBePlacedAt(locationTo) << endl;
        logger.debugLog << state.grid[locationTo.row][locationTo.col] << endl;
        #endif

        if (!Ant::canBePlacedAt(locationTo)) {
            #ifdef __DEBUG
            logger.debugLog << *this << " My path got blocked at " << LocationToString(locationTo) << " while trying to make a path move" << endl;
            #endif
            if (path->hasTarget()) {
                #ifdef __DEBUG
                logger.debugLog << *this << " Blurring food at " << LocationToString(path->targetLocation()) << endl;
                #endif
                path->targetLocation().blurFood();
            }
            Ant::deletePath();

            if (solveCollision) {
                Ant::prepareMove();

                #ifdef __DEBUG
                profiler.endThinkTime("Ant::getNextMove()");
                codeDepth--;
                #endif

                return Ant::getNextMove(false);
            }
        }

        velocity = vector2f(relLoc.col, relLoc.row);

        if (path && path->stepsLeft() >= 1) path->moves.pop();
        /* crashy?
        if (path && path->stepsLeft() >= 1) {
            if (path->stepsLeft() == 1 || locationTo.isAround(path->targetLocation()) || path->moves.top().damageArea().enemy >= 1) {
                if (path->hasTarget()) path->targetLocation().blurFood();
                Ant::deletePath();
            }
        }
        */
    }

    physicalPosition = vector2f(getLocation().col + velocity.x, getLocation().row + velocity.y);

    const Location& nearestFood = Ant::getLocation().nearestFood();
    double distanceToNearestFood = numeric_limits<double>::max();
    bool foodCollision = false;

    if (nearestFood.isValid()) {
        distanceToNearestFood = distance_fast(Ant::getLocation().row, Ant::getLocation().col, nearestFood.row, nearestFood.col);
        foodCollision = nearestFood.collisionLine(Ant::getLocation());
    }

    if (Ant::hasEnoughForceToMove())
    {
        std::vector<directionMove> moves;
/*
        if (Ant::getLocation().damageArea().enemy >= 1 && Ant::getLocation().damageArea().friendly > Ant::getLocation().damageArea().enemy) {
            // loop for multiple?
            const Location& helpingAnt = *Ant::getLocation().damageArea().friendlyAnts.front();
            const Location& enemyAnt = *Ant::getLocation().damageArea().enemyAnts.front();

            double dist = distance_fast(helpingAnt.row, helpingAnt.col, enemyAnt.row, enemyAnt.col);

            if (dist <= (state.attackradius + 1)) {
                noMove = true;
            }
        }
*/
        // Make a list of directions
        for(int dir = -1; dir < 4; dir++)
        {
            const Location& locationTo = state.getLocation(Ant::getLocation(), dir);
            if (locationTo.isType(WALL)) {
                #ifdef __DEBUG
                logger.debugLog << "dir: " << dir << ", blocked" << std::endl;
                #endif
                continue;
            }
/* crashy?
            if (Ant* nAnt = Ant::getLocation().nearestAnt((AntFlags)(ANT_ISFRIEND + ANT_HASPATH), Ant::getLocation(), 2).getAnt()) {
                if (nAnt && nAnt->path && nAnt->path->stepsLeft() >= 1 && locationTo.equals(nAnt->path->moves.top())) {
                    #ifdef __DEBUG
                    logger.debugLog << "dir: " << dir << ", path blocking" << std::endl;
                    #endif
                    continue;
                }
            }
*/
            int score = 0;

            directionMove move;
            move.direction = dir;

            if (dir == NO_MOVE) {
                move.cost = 100;
            } else {
                move.cost = gameMap.distance_vector(locationTo, physicalPosition);
                if (gameMap.isDeadEnd(Ant::getLocation(), dir)) move.cost *= 10;
            }

            if (locationTo.damageArea().enemy == 0) {
                const Location& nFood = locationTo.nearestFood();
                if (nFood.isValid()) {
                    /*
                    if nearest enemy is in range of picking the food && we are further than him, don't move that direciton
                    */
                    if (nFood.nearestAnt(ANT_ISFRIEND).equals(Ant::getLocation())) {
                        double d = distance_fast(nFood.row, nFood.col, locationTo.row, locationTo.col);
                        if (d <= state.spawnradius) {
                            move.cost += -80000 + d;
                            moves.push_back(move);

                            #ifdef __DEBUG
                            logger.debugLog << "dir: " << dir << ", special move to food (no damage), cost: " << move.cost << std::endl;
                            #endif

                            continue;
                        }
                    }
                }
            } else
            if (locationTo.damageArea().enemy >= 1) {
                const Location& nFood = locationTo.nearestFood();
                if (nFood.isValid()) {
                    const bool nCollision = locationTo.collisionLine(nFood);
                    double d = distance_fast(nFood.row, nFood.col, locationTo.row, locationTo.col);
                    if (
                        (
                            (d <= state.spawnradius) ||
                            (d <= state.attackradius && !nCollision)
                        ) &&
                        (
                            (nFood.damageArea().enemy == 0) ||
                            (nFood.damageArea().enemy >= 1 && nFood.damageArea().enemyDistance >= d)
                            // todo: only risk losing ant if there is another one around
                            //       we want to prevent enemy from picking it
                        ) &&
                        (
                            (gameMap.findMany(nFood, state.viewradius, ANT_FRIENDLY).size() > gameMap.findMany(nFood, state.viewradius, ANT_ENEMY).size())
                        )
                    ) {
                    //if (d <= state.spawnradius) {
                        move.cost = -50000 + (d * 100);
                        if (nFood.damageArea().enemy >= 1 && nFood.damageArea().enemyDistance >= d) {
                            move.cost -= locationTo.damageArea().enemyDistance;
                        }

                        #ifdef __DEBUG
                        logger.debugLog << "dir: " << dir << ", special move to food (with damage), cost: " << move.cost << std::endl;
                        #endif

                        moves.push_back(move);
                        continue;
                    }
                }
                for (int enemy_id = 0; enemy_id < locationTo.damageArea().enemyAnts.size(); enemy_id++) {
                    const Location& nearEnemy = (*locationTo.damageArea().enemyAnts[enemy_id]);

                    for(int enemyDir = 0; enemyDir < 4; enemyDir++)
                    {
                        const Location& nearEnemyPrediction = state.getLocation(*locationTo.damageArea().enemyAnts[enemy_id], enemyDir);
                        if (nearEnemyPrediction.isType(WALL)) continue;

                        int extraDamage = (distance_fast(nearEnemyPrediction.row, nearEnemyPrediction.col, Ant::getLocation().row, Ant::getLocation().col) <= state.attackradius) ? 0 : 1;

                        // also use enemy position to support many ants
                        nearEnemy.damageArea().sideDamage[enemyDir] = 0;

                        std::vector<const Location*> friendlyAnts = gameMap.findMany(nearEnemyPrediction, sqrt(sqr(state.attackradius) + 5), ANT_FRIENDLY);

                        for (int friend_id = 0; friend_id < friendlyAnts.size(); friend_id++) {
                            const Location& friendlyAntLocation = *friendlyAnts[friend_id];
                            if (Ant* friendlyAnt = gameMap.getAntAt(friendlyAntLocation)) {
                                // make the ant think if unknown move
                                if (this != friendlyAnt && friendlyAnt->predictionMove == MOVE_UNKNOWN) {
                                    friendlyAnt->onThink();
                                }

                                if (this != friendlyAnt && friendlyAnt->predictionMove != MOVE_UNKNOWN) {
                                    const Location& nextMove = state.getLocation(friendlyAnt->getLocation(), friendlyAnt->predictionMove);
                                    if (
                                        (distance_fast(nearEnemyPrediction.row, nearEnemyPrediction.col, nextMove.row, nextMove.col) <= state.attackradius)
                                        &&
                                        (distance_fast(nearEnemyPrediction.row, nearEnemyPrediction.col, friendlyAnt->getLocation().row, friendlyAnt->getLocation().col) > state.attackradius)
                                    ) {
                                        extraDamage++;
                                    }

                                    if (distance_fast(nearEnemyPrediction.row, nearEnemyPrediction.col, nextMove.row, nextMove.col) <= state.attackradius) {
                                        #ifdef __DEBUG
                                        logger.debugLog << "Adding side damage " << enemyDir << " to enemy at " << LocationToString(nearEnemy) << std::endl;
                                        #endif
                                        nearEnemy.damageArea().sideDamage[enemyDir]++;
                                    }

                                }
                            }
                        }

                        if (distance_fast(nearEnemyPrediction.row, nearEnemyPrediction.col, locationTo.row, locationTo.col) <= state.attackradius) {
                            if ((nearEnemyPrediction.damageArea().friendly + extraDamage) > nearEnemyPrediction.damageArea().enemy) {
                                //score += (nearEnemyPrediction.damageArea().friendly + extraDamage) - nearEnemyPrediction.damageArea().enemy;
                                score++;
                            } else
                            if ((nearEnemy.damageArea().sideDamage[enemyDir] + extraDamage) > locationTo.damageArea().enemy) {
                                //score += nearEnemy.damageArea().sideDamage[enemyDir] - locationTo.damageArea().enemy;
                                score++;
                            } else {
                                score--;
                            }
                        }
                    }
                }


                // or count grid within each of his prediction
                if (score >= 1) { // 1
                //if (locationTo.damageArea().friendly > locationTo.damageArea().enemy && locationTo.countAnts(state.attackradius, false) >= locationTo.damageArea().enemy) { // 2
                //if (locationTo.damageArea().friendly > locationTo.damageArea().enemy && locationTo.countAnts(state.attackradius, false) >= locationTo.damageArea().enemy && nearEnemy.countAnts(state.attackradius + 1, false) >= locationTo.damageArea().enemy) { // 3
                    // to be safe check also self ants if equal to friendly? ^

                    // lets kill an ant for free, lets make the step easy to move
                    /*
                    #ifdef __DEBUG
                    if (!bot.hasAggressiveMode()) {
                        logger.debugLog << "Killing ant efficiently at " << LocationToString(locationTo) << std::endl;
                        logger.debugLog << "my damage: " << locationTo.damageArea().friendly << ", enemy damage: " << locationTo.damageArea().enemy << std::endl;
                    }
                    #endif

                    //move.cost = (2000 * (locationTo.damageArea().enemy - locationTo.damageArea().friendly)) + locationTo.damageArea().enemyDistance;
                    move.cost = (20000 * (locationTo.damageArea().enemy - locationTo.damageArea().friendly)) + locationTo.damageArea().enemyDistance;
                    effectiveKill = true;*/

                    double friendDistance = 0;

                    for (int friend_id = 0; friend_id < locationTo.damageArea().friendlyAnts.size(); friend_id++) {
                        const Location& friendlyAntLocation = *locationTo.damageArea().friendlyAnts[friend_id];
                        if (!friendlyAntLocation.equals(Ant::getLocation())) {
                            friendDistance = distance_fast(locationTo.row, locationTo.col, friendlyAntLocation.row, friendlyAntLocation.col);
                            break;
                        }
                    }

                    move.cost = -10000 - (score * 1000) + (locationTo.damageArea().enemyDistance * 100) + (friendDistance * 10);
/*
                    if (locationTo.damageArea().friendly > locationTo.damageArea().enemy) {
                        move.cost -= (locationTo.damageArea().friendly - locationTo.damageArea().enemy) / 10;
                    }
*/

                } else
                if (dir != NO_MOVE) {
                    // we dont want to die, lets make the step harder to move (most furthest from enemy ant)
                    //move.cost = 10000 - (locationTo.damageArea().enemyDistance * 10);
                    move.cost = 10000 - (score * 1000) - (locationTo.damageArea().enemyDistance * 100) /* - nearest wall distance */;
                }
                //move.cost += (10000 * locationTo.damageArea().enemy) - (locationTo.damageArea().enemyDistance * 10);

                #ifdef __DEBUG
                logger.debugLog << "dir: " << dir << ", damage: " << locationTo.damageArea().enemy << " vs " << locationTo.damageArea().friendly << ", dist: " << locationTo.damageArea().enemyDistance << ", cost: " << move.cost << ", score: " << score << std::endl;
                #endif
            }

            moves.push_back(move);
        }

        if (moves.size() >= 1) {
            std::sort(moves.begin(), moves.end(), directionMove::sortMoves());

            for (vector<directionMove>::iterator move = moves.begin(); move != moves.end(); ++move)
            {
                predictionMove = (*move).direction;


                const Location& locationTo = state.getLocation(Ant::getLocation(), (*move).direction);
                if (locationTo.damageArea().enemy >= 1 && locationTo.damageArea().enemyDistance == 1) {
                    continue;
                    #ifdef __DEBUG
                    profiler.endThinkTime("Ant::getNextMove()");
                    codeDepth--;
                    #endif
                    predictionMove = MOVE_DANAGEROUS;
                    return NO_MOVE;
                }

                Ant* forwardAnt = locationTo.getAnt();
                locationTo.think();

                if (forwardAnt && forwardAnt->predictionMove == MOVE_DANAGEROUS) {
                    continue;
                    #ifdef __DEBUG
                    profiler.endThinkTime("Ant::getNextMove()");
                    codeDepth--;
                    #endif
                    predictionMove = MOVE_DANAGEROUS;
                    return NO_MOVE;
                }

                if (Ant::canBePlacedAt(locationTo)) {
                    ret = (*move).direction;
                }
            }
        }
    }


    const Location& locationTo = state.getLocation(Ant::getLocation(), ret);

/*
    if (path) {
        if (path->stepsLeft() >= 1) path->moves.pop();

        if (path->stepsLeft() == 0 || locationTo.isAround(path->targetLocation())) {
            if (path->hasTarget()) path->targetLocation().blurFood();
            Ant::deletePath();
        }
    }
*/

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

    // this should be replaced by genetic algorithm (if done)
    if (nearestFood.isValid() && nearestFood.isAround(Ant::getLocation()) && !nearestFood.isAround(locationTo)) {
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
	out << "[ANT #" << (int)ant.id << "] " << LocationToString(ant.getLocation());
	#endif
	return out;
}
