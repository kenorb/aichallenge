#include "State.h"

struct State;

#ifndef ANT_H
#define ANT_H

class Ant
{
    public:
        Ant(State &_state, Location &_loc);
        void onMove(Location& toLoc);
        bool canBePlacedAt(Location& loc);
        int getNextMove();
        virtual ~Ant();

        Location loc;

    protected:
    private:
        State* state;
};

#endif // ANT_H
