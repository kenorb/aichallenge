#include <iostream>
#include <fstream>
#include <sstream>

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
        s << (int)row << " x " << (int)col;
        return s.str();
    };
};

#endif //LOCATION_H_
