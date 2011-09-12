#ifndef LOCATION_H_
#define LOCATION_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

#include "Ant.h"
#include "Damage.h"
#include "Map.h"

#include "const.h"
#include "globals.h"
#include "enums.h"

#define LOCATION_UNDEFINED (*locNull);

class Ant;
class Path;

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


    int countAnts(double radius, bool predict = false) const;
    Damage& damageArea() const;

    vector2f getForce(Ant* forAnt, bool attraction = true, bool repulsion = true) const;
    Path* findPathTo(const Location& endLocation) const;
    //foodList foodInRadius(double radius) const;
    const Location& nearestFood(bool focusedFood = false) const;

    const Location& nearestAnt(int antFlags = ANT_NOFLAGS, const Location& ignoreAnt = *locNull) const;
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
    Path() { iterations = totalCost = 0; };
    std::stack<Location> moves;

    int totalCost;
    int iterations;

    inline int stepsLeft() { return moves.size(); };

    inline const Location& targetLocation() { return *target; };
    inline const Location& sourceLocation() { return *source; };

    inline bool hasTarget() { return target; };
    inline bool hasSource() { return source; };

    inline void setTarget(const Location& loc) { target = &loc; };
    inline void setSource(const Location& loc) { source = &loc; };

    private:
        const Location* target;
        const Location* source;
};

struct SearchLocation
{
    SearchLocation(const Location& _loc, int _cost);
    void closeSquare() { opened = false; };
    SearchLocation* setRef(const Location& location) { ref = &location; return this; };

    inline void setLocation(const Location& location) { loc = &location; };
    inline const Location& getLocation() { return *loc; };
    inline const Location& getReference() { return *ref; };

    int cost;
    double opened;

    private:
        const Location* loc;
        const Location* ref;
};

#endif //LOCATION_H_
