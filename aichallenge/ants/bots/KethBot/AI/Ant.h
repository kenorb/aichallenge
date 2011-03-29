#include "State.h"

struct State;

#ifndef ANT_H
#define ANT_H

struct Ant
{
    public:
        Ant(State &_state, Location &_loc);
        ~Ant();

        void onThink();
        void onMove(Location& toLoc);
        bool canBePlacedAt(Location& loc);
        int getNextMove();
        int timeAlive;
        int lastThink;

        Location getLocation()
        {
            return loc;
        }



        int id;

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
