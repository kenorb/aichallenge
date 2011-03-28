#include "State.h"

struct State;

#ifndef ANT_H
#define ANT_H

using namespace std;

struct Ant
{
    public:
        Ant(State &_state, Location &_loc);
        ~Ant();

        void onMove(Location& toLoc);
        bool canBePlacedAt(Location& loc);
        int getNextMove();


        Location loc;
        int id;
        bool hasMoved;

    protected:
    private:
        State* state;
};

ostream& operator<<(ostream &out, const Ant &ant);

#endif // ANT_H
