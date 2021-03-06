#ifndef LOCATION_H_
#define LOCATION_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

#include "Ant.h"
#include "Damage.h"
#include "State.h"

#include "const.h"
#include "globals.h"
#include "enums.h"

#define LOCATION_UNDEFINED (*locNull);
#define cachedNode(a,b) (optimizer.distance_cost_table[a][b] > 0 ? optimizer.distance_cost_table[a][b] : optimizer.distance_cost_table[b][a])

#define RefSource(locationRef) (*locationRef->ref)
#define RefLocation(locationRef) (*locationRef->loc)
#define RefDistance(locationRef) (distance_fast(locationRef->loc->row, locationRef->loc->col, locationRef->ref->row, locationRef->ref->col))


#define Loc(row, col) gameMap.locationGrid[row][col]
#define LocCompare(loc1, loc2) (loc1.row == loc2.row && loc1.col == loc2.col)

#define Row(loc) (loc->row)
#define Col(loc) (loc->col)

#define LocIsAround(a, b) (distance_fast(a.row, a.col, b.row, b.col) < state.spawnradius)

#define LocIsFog(row, col) (state.grid[row][col] == '?')
#define LocIsWall(row, col) (state.grid[row][col] == '%')
#define LocIsEmpty(row, col) (state.grid[row][col] == '.' || state.grid[row][col] == 'o')
#define LocType(loc) state.grid[loc.row][loc.col]
#define LocCache(loc) state.locCache[loc.row][loc.col]

#define LocFindFood(loc, type, index) (LocCache(loc).nearestFood[type][index])
#define LocFindAnt(loc, type, index) (LocCache(loc).nearestAnt[type][index])

#define didFind(x) (x.location != NULL)

class Ant;
class Path;

#include "State.h"

struct CallbackLocData;

void onTileCallback(const Location& loc, CallbackLocData data);

struct Location
{
    int row, col, index;

    Location() { row = col = 0; };
    Location(int r, int c);
    Location(vector2f vec);

    bool isType(const LocationType type) const;

    inline bool equals(const Location& loc) const { return row == loc.row && col == loc.col; }
    bool isValid() const;
    int getIndex();

    bool isAround(const Location& loc) const;
    void normalize();
    DamageSolve solveAttack();

    bool cacheElement(LocationRef& cache, const Location& element, const Location& loc) const;
    void cleanCache() const;
    LocationCache& updateCache() const;
    #ifdef __ASSERT
    void validateCache(std::string type, const Location* cacheLoc, const Location& foundLoc) const;
    #endif


    int countAnts(double radius, bool predict = false) const;
    Damage& damageArea() const;


    vector2f getForce(Ant* forAnt, bool attraction = true, bool repulsion = true) const;
    Path* findPathTo(const Location& endLocation, bool costOnly = false) const;
    //foodList foodInRadius(double radius) const;
    const Location& nearestFood(bool focusedFood = false) const;

    const Location& nearestAnt(int antFlags = ANT_NOFLAGS, const Location& ignoreAnt = *locNull, const double searchRadius = 32) const;
    Location relativeLocationTo(const Location& loc) const;
    double costTo(const Location& loc, bool precise = false) const;
    bool collisionLine(const Location& loc, bool secondSolve = false) const;

    void onUpdate() const;
    void onAppear() const;
    void onDisappear() const;

    bool hasAnt() const;
    Ant* getAnt() const;
    bool isWall() const;
    bool hasFood() const;
    bool hasFreshFood();
    bool isBlocked() const;
    void focusFood() const;
    void blurFood() const;
    bool think() const;

    #ifdef __DEBUG
    void addDebugLine(std::string line);
    #endif

};

struct Food
{
    Location location;
    double attractionForce;
};

#endif //LOCATION_H_
