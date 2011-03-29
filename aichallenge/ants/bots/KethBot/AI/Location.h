#include <iostream>
#include <fstream>
#include <sstream>
#include "globals.h"

#ifndef LOCATION_H_
#define LOCATION_H_

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
        s << "[POS " << (int)row << " x " << (int)col << "]";
        return s.str();
    };

    bool think();

};
#endif //LOCATION_H_
