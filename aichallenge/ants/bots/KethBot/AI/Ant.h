#include "State.h"
#include "vector2d.h"

struct State;

#ifndef ANT_H
#define ANT_H

struct Ant
{
    public:
        Ant(State &_state, Location &_loc);
        ~Ant();

        vector2f physicalPosition;
        vector2f velocity;

        void onThink();
        void onMove(Location& toLoc);
        bool canBePlacedAt(Location& loc);
        void prepareMove();
        int getNextMove();
        int timeAlive;
        int lastThink;

        bool hasEnoughForceToMove();
        void updatePriority();
        int movePriority;

        list<Location> foodQueue;

        Location getLocation()
        {
            return loc;
        }

        int id;
        int temporaryId;
        void updateTemporaryId(int _x) { temporaryId = _x; }

        // TODO: hasMoved should be used implemented into movement solver
        // looping through ants that did not move and asking them for the
        // next movement again.
        bool hasMoved;

    protected:

    private:
        Location loc;
};

ostream& operator<<(ostream &out, const Ant &ant);

#endif // ANT_H
