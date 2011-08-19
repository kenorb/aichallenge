#include <iostream>
#include <fstream>
#include <sstream>
#include "globals.h"
#include <stack>

#define LOCATION_UNDEFINED locNull;

#ifndef LOCATION_H_
#define LOCATION_H_

struct Ant;

struct Path;

struct Location
{
    int row, col;

    Location() { row = col = 0; };
    Location(int r, int c);
    Location(vector2f vec);

    bool isValid();
    int getIndex();
    bool equals(const Location& loc) { return row == loc.row && col == loc.col; }
    bool isAround(const Location& loc);
    double distanceTo(const Location& loc);
    void normalize();
/*
    std::string str()
    {
        std::stringstream s;
        s << "[POS " << row << " x " << col << "]";
        return s.str();
    };
*/

    vector2f getForce(Ant* forAnt);
    Path* findPathTo(const Location& endLocation);
    Location& nearestFood();
    Ant* nearestAnt(bool ignoreTakenAnts = false);
    Location relativeLocationTo(const Location& loc);
    double costTo(const Location& loc, bool precise = false);
    bool collisionLine(const Location& loc, bool secondSolve = false);

    bool hasAnt();
    bool hasWall();
    bool hasFood();
    bool isBlocked();
    void focusFood();
    void blurFood();
    bool think();

    #ifdef __DEBUG
    void addDebugLine(std::string line);
    #endif

};

struct Path
{
    Path() { iterations = totalCost = 0; };
    stack<Location> moves;

    int totalCost;
    int iterations;
    bool found;

    virtual Location& targetLocation() { return *target; };
    virtual Location& sourceLocation() { return *source; };

    virtual void setTarget(Location& loc) { target = &loc; };
    virtual void setSource(Location& loc) { source = &loc; };

    private:
        Location* target;
        Location* source;
};

struct SearchLocation
{
    SearchLocation(Location& _loc, int _cost);
    void closeSquare() { opened = false; };
    SearchLocation* setRef(Location* location) { ref = location; return this; };
    Location loc;
    Location* ref;
    int cost;
    double opened;
};

#endif //LOCATION_H_
