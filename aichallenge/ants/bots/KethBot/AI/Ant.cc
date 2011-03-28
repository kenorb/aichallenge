#include "Ant.h"
#include "Logger.h"
#include "Location.h"
#include "const.h"

// TODO:
// * Calculate accurate percent of discovered map

Ant::Ant(State &_state, Location &_loc)
{
    state = &_state;
    loc.col = _loc.col;
    loc.row = _loc.row;
    state->structuralAnts.push_back(this);

    #ifdef __DEBUG
    state->logger.debugLog << "EVENT: New ant at " << loc.str() << endl;
    #endif
}

Ant::~Ant()
{
    #ifdef __DEBUG
    state->logger.debugLog << "EVENT: Ant dies at " << loc.str() << endl;
    #endif

    state->structuralAnts.remove(this);
}

void Ant::onMove(Location& toLoc)
{
    #ifdef __DEBUG
    state->logger.debugLog << "EVENT: Ant moves from " << loc.str() << " to " << toLoc.str() << endl;
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

int Ant::getNextMove()
{
    // Move into some direction
    for(int d=0; d<4; d++)
    {
        Location locTo = state->getLocation(loc, d);

        if(Ant::canBePlacedAt(locTo))
        {
            return d;
            break;
        }
    }

    // Don't move
    return NULL;
}

