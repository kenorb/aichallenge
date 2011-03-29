#include "Ant.h"
#include "Logger.h"
#include "Location.h"
#include "const.h"

// TODO STRATEGY:
// * Calculate accurate percent of *discovered map*
// * Calculate accurate percent of *area covered*
// * Calculate for each ant average *push force* against all the other ants and walls
// * Push each ant by *invidual force* according to the *discovered map* in order to keep the *50 percent* balance

// OBJECTS
// * Every object pushes other objects according to it's own *mass value*
//    - walls having lower mass than ants (according to number of walls?)

int totalAnts = 0;

Ant::Ant(State &_state, Location &_loc)
{
    id = totalAnts;
    totalAnts++;

    timeAlive = 0;
    lastThink = 0;

    state = &_state;
    loc.col = _loc.col;
    loc.row = _loc.row;
    state->structuralAnts.push_back(this);

    #ifdef __DEBUG
    state->logger.debugLog << "EVENT: New " << (*this) << " at " << loc.str() << endl;
    #endif
}

Ant::~Ant()
{
    #ifdef __DEBUG
    state->logger.debugLog << "EVENT: " << (*this) << " dies at " << loc.str() << endl;
    #endif

    state->structuralAnts.remove(this);
}

void Ant::onMove(Location& toLoc)
{
    #ifdef __DEBUG
    state->logger.debugLog << "EVENT: " << (*this) << " moves from " << loc.str() << " to " << toLoc.str() << endl;
    #endif

    loc.col = toLoc.col;
    loc.row = toLoc.row;
}

bool Ant::canBePlacedAt(Location& loc)
{
    if(state->grid[loc.row][loc.col] == '%') return false;
    if(state->grid[loc.row][loc.col] == 'a') return false;

    return true;
}

void Ant::onThink()
{
    if (lastThink == state->turn) return;
    lastThink = state->turn;

    int nextMove = getNextMove();
    if (nextMove != NO_MOVE) state->makeMove(getLocation(), nextMove);

    timeAlive++;
}

int Ant::getNextMove()
{
    int ret = NO_MOVE;

    // Move into some direction
    for(int d=0; d<4; d++)
    {
        Location locationTo = state->getLocation(loc, d);

        // Solve the location before we move there
        locationTo.think();

        if(Ant::canBePlacedAt(locationTo))
        {
            ret = d;
            break;
        }
    }

    if (ret != NO_MOVE) hasMoved = true;
    return ret;
}


ostream& operator<<(ostream &out, const Ant& ant)
{
	out << "[ANT #" << (int)ant.id << "]";
	return out;
}

