#include "State.h"

struct State;

#ifndef ANT_H
#define ANT_H

class Ant
{
    public:
        Ant(State &_state, Location &_loc);
        virtual ~Ant();

        Location loc;

    protected:
    private:
        State* state;
};

#endif // ANT_H
