#ifndef PATH_H
#define PATH_H

//#include "Location.h"

#include <stack>

struct Location;

struct Path
{
    Path() { iterations = totalCost = 0; touchedFog = false; };
    std::stack<const Location*> moves;

    int totalCost;
    int iterations;
    bool touchedFog;

    inline int stepsLeft() { return moves.size(); };

    inline const Location& targetLocation() { return *target; };
    inline const Location& sourceLocation() { return *source; };

    inline bool hasTarget() { return target; };
    inline bool hasSource() { return source; };

    inline void setTarget(const Location& loc) { target = &loc; };
    inline void setSource(const Location& loc) { source = &loc; };

    const Location* target;
    const Location* source;
};

struct SearchLocation
{
    int cost;
    double opened;
    double distanceLeft;

    const Location* loc;
    const Location* ref;
};

#endif // PATH_H
