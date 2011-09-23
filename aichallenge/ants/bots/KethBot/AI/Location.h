#ifndef LOCATION_H_
#define LOCATION_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

#include "Ant.h"
#include "Damage.h"
#include "Map.h"
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

class Ant;
class Path;

#include "State.h"


struct LocationRef
{
    LocationRef() {};
    LocationRef(const Location& _loc, const Location& _ref) {
        loc = &_loc;
        ref = &_ref;
    };

    const Location* loc;
    const Location* ref;
};

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

struct Path
{
    Path() { iterations = totalCost = 0; touchedFog = false; };
    std::stack<Location> moves;

    int totalCost;
    int iterations;
    bool touchedFog;

    inline int stepsLeft() { return moves.size(); };

    inline const Location& targetLocation() { return *target; };
    inline const Location& sourceLocation() { return *source; };

    inline bool hasTarget() { return target; };
    inline bool hasSource() { return source; };

    inline void setTarget(const Location& loc) { target = &loc; };
    inline void setSource(const Location& loc) { source = &loc; };

    const Location* target;
    const Location* source;
};

struct SearchLocation
{
    int cost;
    double opened;
    double distanceLeft;

    const Location* loc;
    const Location* ref;
};

#endif //LOCATION_H_
