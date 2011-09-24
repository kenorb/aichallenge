#ifndef ENUMS_H_INCLUDED
#define ENUMS_H_INCLUDED

#include "const.h"

enum AntFlags {
    ANT_HASPATH         = 1,
    ANT_NOPATH          = 2,
    ANT_ISENEMY         = 4,
    ANT_ISFRIEND        = 8,
    ANT_UNDERATTACK     = 16,
    ANT_SEENENEMY       = 32,
    ANT_DIDMOVE         = 64,
    ANT_DIDTHINK        = 128,
    ANT_NOFLAGS         = (256 - 1),
};

enum LocationType {
    ANT,
    ANT_FRIENDLY,
    ANT_ENEMY,
    FOOD,
    FOOD_FOCUSED,
    FOOD_BLURRED,
    WALL,
    FOG,
    EMPTY,

    LOCATION_TYPE_MAX
};

struct LocationRef
{
    LocationRef() {};
    LocationRef(const Location& _loc, const Location& _ref) {
        location = &_loc;
        ref = &_ref;
    };

    const Location* location;
    const Location* ref;

    bool operator () (const LocationRef* loc1) const
    {
        return true;
    }

    double distance;
};

struct LocationCache
{
    LocationRef nearestFood[FOOD_TYPES_COUNT][MAX_CACHE_DEPTH]; // represents ANY, FOCUSED, BLURRED
    LocationRef nearestAnt[ANT_TYPES_COUNT][MAX_CACHE_DEPTH]; // represents ANY, FRIEND, ENEMY

    uint8 nearestFoodSize[FOOD_TYPES_COUNT];
    uint8 nearestAntSize[ANT_TYPES_COUNT];

    int turn;
};

#endif // ENUMS_H_INCLUDED
