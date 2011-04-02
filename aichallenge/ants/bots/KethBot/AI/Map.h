#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "Ant.h"
#include "globals.h"

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


        std::list<Ant*>& getAnts();
        vector<Location>& getEnemyAnts();

        Ant* getAntAt(const Location &loc);
        Ant* setAntAt(const Location &loc, Ant* ant);

        double distance(const Location &loc1, const Location &loc2);
        double distance_vector(const Location &loc1, vector2f loc2);
};

#endif // MAP_H_INCLUDED
