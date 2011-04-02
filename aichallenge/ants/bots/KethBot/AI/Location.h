#include <iostream>
#include <fstream>
#include <sstream>
#include "globals.h"

#ifndef LOCATION_H_
#define LOCATION_H_

struct Ant;

struct Location
{
    int row, col;

    Location()
    {
        row = col = 0;
    };

    Location(int r, int c)
    {
        row = r;
        col = c;
    };

    std::string str()
    {
        std::stringstream s;
        s << "[POS " << row << " x " << col << "]";
        return s.str();
    };

    Ant* nearestAnt(bool checkWalls = false);
    Location relativeLocationTo(Location& loc);
    double distanceTo(Location& loc2);

    bool hasAnt();
    bool think();

    #ifdef __DEBUG
    void addDebugLine(std::string line);
    #endif

};

#endif //LOCATION_H_
