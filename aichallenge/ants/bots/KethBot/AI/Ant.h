struct State;

#ifndef ANT_H
#define ANT_H

#include "State.h"
#include "Location.h"
#include "enums.h"

#include <iostream>
#include <functional>

struct Location;
struct Path;

using namespace std;

struct Ant
{
    public:
        Ant(State &_state, const Location &_loc);
        ~Ant();

        vector2f physicalPosition;
        vector2f velocity;

        bool hasFlags(AntFlags flag);
        AntFlags getFlags();

        void onThink();
        void onNewTurn();
        void onMove(const Location& toLoc);
        bool canBePlacedAt(const Location& loc);
        void prepareMove();
        int getNextMove(bool solveCollision = true);
        int timeAlive;
        int lastThink;

        const Location* nearestEnemyAnt;

        double distanceTo(const Location& loc2);
        double distanceTo(Ant* ant);
        bool hasEnoughForceToMove();
        void updatePriority();
        int movePriority;
        double targetDistanceMul;

        void setPathTo(const Location& target);
        void deletePath();



        const Location& getLocation() const { return *loc; }



        int id;
        int temporaryId;
        int moveIterations;
        int predictionMove;
        void updateTemporaryId(int _x) { temporaryId = _x; };

        // TODO: hasMoved should be used implemented into movement solver
        // looping through ants that did not move and asking them for the
        // next movement again.
        bool hasMoved;
        bool seenEnemy;

        Path* path;
        Ant* partner;

        inline bool hasPath() { return path != NULL; };
        inline bool hasPartner() { return partner != NULL; };

        const Location* loc;
};

struct directionMove {
    int direction;
    double cost;
/*
    inline bool operator>(const directionMove& a)
    {
        return (*this).cost < a.cost;
    }

    inline bool operator<(const directionMove& a)
    {
        return (*this).cost > a.cost;
    }
*/
    struct sortMoves: public binary_function<directionMove, directionMove, bool>
    {
        bool operator() (directionMove move1, directionMove move2)
        {
            return (move1.cost > move2.cost);
        }
    };

};


ostream& operator<<(ostream &out, const Ant &ant);

#endif // ANT_H
