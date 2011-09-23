struct State;

#ifndef ANT_H
#define ANT_H

#include "State.h"
#include "Location.h"
#include "enums.h"

#include <iostream>
#include <functional>

#define AntRow(x) x->loc->row
#define AntCol(x) x->loc->col
#define AntLoc(x) (*x->loc)

struct Location;
struct Path;

using namespace std;

struct directionMove {
    int direction;
    double cost;

    struct sortMoves: public binary_function<directionMove, directionMove, bool>
    {
        bool operator() (directionMove move1, directionMove move2)
        {
            return (move1.cost < move2.cost);
        }
    };

};

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
        bool isEnemyInRange(const double radius);
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


        int scoreMove(directionMove& move);
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

        void cancelPath();
        bool needsCancelPath();

        void updatePath();

        const Location* loc;
};



ostream& operator<<(ostream &out, const Ant& ant);

#endif // ANT_H
