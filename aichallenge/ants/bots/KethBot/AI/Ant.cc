#include "Ant.h"
#include "Logger.h"
#include "Location.h"


// TODO:
// * Accurate percent of discovered map

Ant::Ant(State &_state, Location &_loc)
{
    state = &_state;
    loc.col = _loc.col;
    loc.row = _loc.row;


    //state->debugLog << "EVENT (Turn #" << state->turn << "): New ant at " << (int)loc.row << "x" << (int)loc.col << endl;
}

Ant::~Ant()
{
    //state->debugLog << "EVENT (Turn #" << state->turn << "): Ant dies at " << (int)loc.row << "x" << (int)loc.col << endl;
}
