#include "Ant.h"
#include "Logger.h"
#include "Location.h"
#include "const.h"
#include "Bot.h"
#include "Map.h"

#define pi 3.14159
#define sqr(x) (x*x)

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

    timeAlive = 0;
    lastThink = 0;
    movePriority = 0;

    loc.col = _loc.col;
    loc.row = _loc.row;
    state.structuralAnts.push_back(this);

    physicalPosition((double)loc.col, (double)loc.row);
    velocity(0,0);

    #ifdef __DEBUG
    logger.debugLog << "EVENT: New " << (*this) << " at " << loc.str() << endl;
    #endif

    Ant::updatePriority();
}

Ant::~Ant()
{
    #ifdef __DEBUG
    logger.debugLog << "EVENT: " << (*this) << " dies at " << loc.str() << endl;
    #endif

    state.structuralAnts.remove(this);
}

void Ant::updatePriority()
{
    movePriority = gameMap.distance(Ant::getLocation(), bot.startLocation);
}

void Ant::onMove(Location& toLoc)
{
    #ifdef __DEBUG
    logger.debugLog << "EVENT: " << (*this) << " moves from " << loc.str() << " to " << toLoc.str() << endl;
    #endif

    loc.col = toLoc.col;
    loc.row = toLoc.row;
}

bool Ant::canBePlacedAt(Location& loc)
{
    if (state.grid[loc.row][loc.col] == '%') return false;
    if (state.grid[loc.row][loc.col] == 'a') return false;
    if (state.grid[loc.row][loc.col] == '*') return false; // Server is blocking? Ants die when moved inside food?
    if (gameMap.getAntAt(loc)) return false;

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
    if (nextMove != NO_MOVE) state.makeMove(getLocation(), nextMove);

    timeAlive++;
}

bool Ant::hasEnoughForceToMove()
{
    double velocity = gameMap.distance_vector(Ant::getLocation(), physicalPosition);
    return velocity > 0;
}

void Ant::prepareMove()
{
    double force_x = 0.0f;
    double force_y = 0.0f;

    double expandForce = bot.getExpandForce();

    for(int row=0; row<state.rows; row++)
    for(int col=0; col<state.cols; col++)
    {
        if (getLocation().row == row && getLocation().col == col) continue;

        bool magnetic = false;
        double magnetPower;
        double extraDistance = 0.0f;
        double distanceDivision = 5.0f;

        if(state.grid[row][col] == 'a') {
            magnetic = true;
            magnetPower = 0.5;
            distanceDivision = 15;
        }

        if(state.grid[row][col] == '%') {
            magnetic = true;
            magnetPower = 1;
            distanceDivision = 5;
        }



        if(state.grid[row][col] == '*') {
            Location foodLocation = Location(row, col);
            // TODO: Run path finding algorithm to prevent moving towards wall instead of the food
            if (foodLocation.nearestAnt(true) == this) {
                magnetic = true;
                magnetPower = -2;
                distanceDivision = 15;
            }

        }

        if (magnetic) {
            Location magnetLocation = Location(row, col);

            double distance = gameMap.distance(Ant::getLocation(), magnetLocation) + extraDistance;
            Location relLoc = Ant::getLocation().relativeLocationTo(magnetLocation);

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

    }
    velocity = vector2f(force_x, force_y);

    if (velocity.length() > 5) {
        velocity.normalize();
        velocity *= 5;
    }

    physicalPosition = vector2f(getLocation().col + velocity.x, getLocation().row + velocity.y);

    if (!Ant::hasEnoughForceToMove()) {
        // TODO: Think of something smart to do when not doing a move
    }

    Ant::updatePriority();
}

int Ant::getNextMove()
{
    #ifdef __DEBUG
    logger.debugLog << *this << endl;
    #endif

    int ret = NO_MOVE;

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

