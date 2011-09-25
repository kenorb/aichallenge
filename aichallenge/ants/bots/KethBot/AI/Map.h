#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "Ant.h"
#include "Optimizer.h"

#include "globals.h"
#include "const.h"
#include "enums.h"
#include "allocator.h"
#include "Path.h"

using namespace utils;

class Ant;

const Location& LocWrap(int row, int col);

struct CallbackLocData
{
    const void* sender;
    const Location* senderLocation;
    const relativeLocation* relLoc;
};

typedef void (*CallbackLoc)(const Location&, CallbackLocData data);

struct MapSearch
{
    MapSearch() {
        found = false;
        reachedRadius = false;
        location = locNull;
        index = -1;
    }

    const Location* location;
    int index;
    bool reachedRadius;
    bool found;
};


#define MAX_DISTANCE 255
#define distance_fast(a,b,c,d) optimizer.distance_table[abs(a-c)*state.cols+abs(b-d)]
#define distance_real(a,b,c,d) sqrt(sqr(abs(a-c))+sqr(abs(b-d)))
#define LocToIndex(row, col) ((row)*state.cols+(col))
#define MAX_GRID_INDEX LocToIndex(state.rows-1, state.cols-1)

struct Map
{
    public:
        void onInit();

        void onAnt(int bot_id, const Location &loc);
        void onAntMoves(Ant* ant, const Location &toLocation);
        void onEnemy(const Location &loc);
        void onDeadAnt(int bot_id, const Location &loc);
        void onFood(const Location &loc);
        void onWater(const Location &loc);

        bool isDeadEnd(const Location& loc, int dir);

        void callbackArea(const Location& loc, double radius, CallbackLoc callback, const void* sender = NULL);

        vector<const Location*> findMany(const Location& loc, double searchRadius, LocationType type);

        int stepTowards(const Location& loc1, const Location& loc2, bool forceSearch = false);
        MapSearch find(const Location& loc, double searchRadius, LocationType type, MapSearch next = MapSearch());

        Location** locationGrid;
        SearchLocation* search_grid;
        std::list<Ant*>& getAnts();
        vector<Location>& getEnemyAnts();
        vector<SearchLocation*> opened;

        MemoryPool<Path>* pathPool;

        Ant* getAntAt(const Location &loc);
        Ant* setAntAt(const Location &loc, Ant* ant);

        double distance(int row1, int col1, int row2, int col2);
        double distance(const Location &loc1, const Location &loc2);
        double distance_sqrt(int row1, int col1, int row2, int col2);
        double distance_sqrt(const Location &loc1, const Location &loc2);
        double distance_lut(const Location &loc1, const Location &loc2);
        double distance_table(const Location &loc1, const Location &loc2);
        double distance_vector(const Location &loc1, vector2f loc2);
};

#endif // MAP_H_INCLUDED
