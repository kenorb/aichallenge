#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "Ant.h"

struct Map
{
    public:
        Map();

        void onAnt(int bot_id, Location &loc);
        void onAntMoves(Ant* ant, Location &toLocation);
        void onEnemy(Location &loc);
        void onDeadAnt(int bot_id, Location &loc);
        void onFood(Location &loc);
        void onWater(Location &loc);

        //bool locationThink(const Location& loc);

        Ant* getAntAt(const Location &loc);
        Ant* setAntAt(const Location &loc, Ant* ant);

    private:
        State* state;
};

#endif // MAP_H_INCLUDED
