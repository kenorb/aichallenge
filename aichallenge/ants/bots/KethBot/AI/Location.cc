#include "Location.h"
#include "Ant.h"
#include "Bot.h"

bool Location::think()
{
    if (Ant* ant = gameMap.getAntAt(*this)) {
        ant->onThink();
        return true;
    }
    return false;
}

#ifdef __DEBUG
void Location::addDebugLine(std::string line) {
    char turn[15];
    std::stringstream os;
    sprintf(turn, "%03i", state.turn);

    char pos[32];
    sprintf(pos, "#text_%ix%i", row, col);

    logger.debugNode[turn][pos] = line;
}
#endif

Location Location::relativeLocationTo(Location& loc2)
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
