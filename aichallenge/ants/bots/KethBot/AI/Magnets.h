#ifndef MAGNETS_H
#define MAGNETS_H

#include "Location.h"

struct MagnetData
{
    double power, div, distance;
    bool ignoreDistance;
    Location* forceTowards;
};

void applyMagnetData(MagnetData& magnetData, char& magnetType, const Location& magnetLocation, Ant* ant);

#endif // MAGNETS_H
