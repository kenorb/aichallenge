#include "State.h"
#include "Ant.h"
#include <time.h>

State::State()
{
    gameover = 0;
    errors = 0;
    warnings = 0;
};


void State::logError(std::string error)
{
    logger.debugLog << "[ERROR] [Turn #" << (int)turn << "]: " << error << endl;
    errors++;

}

void State::logWarning(std::string warning)
{
    logger.debugLog << "[ERROR WARNING] [Turn #" << (int)turn << "]: " << warning << endl;
    warnings++;
}

Ant* State::getAntAt(const Location &loc)
{
    if (ants_grid[loc.row][loc.col]) {
        if (Ant* ant = (Ant*)ants_grid[loc.row][loc.col]) {
            return ant;
        }
    }
    return NULL;
}

Ant* State::setAntAt(const Location &loc, Ant* ant)
{
    ants_grid[loc.row][loc.col] = (int)ant;
    return ant;
}

void State::setup()
{
    grid = vector<vector<char> >(rows, vector<char>(cols, '.'));
    ants_grid = vector<vector<int> >(rows, vector<int>(cols, 0));

    cout << "go" << endl;
    turn++;
};

void State::reset()
{
    ants.clear();
    for(int row=0; row<rows; row++)
        for(int col=0; col<cols; col++)
            if(grid[row][col] != '%')
                grid[row][col] = '.';
};

void State::makeMove(const Location &loc, int direction)
{
    #ifdef __DEBUG
    if ((direction < 0) || (direction > 3)) {
        logWarning("Ant trying to move into incorrect direction");
    }

    Location locMoveTo = getLocation(loc, direction);
    if (grid[locMoveTo.row][locMoveTo.col] == '%' || grid[locMoveTo.row][locMoveTo.col] == 'a') {
        logWarning("Ant trying to move inside a wall or another ant");
    }
    #endif

    grid[locMoveTo.row][locMoveTo.col] = 'a';
    grid[loc.row][loc.col] = '.';

    Ant* movingAnt = getAntAt(loc);

    #ifdef __DEBUG
    if (getAntAt(locMoveTo)) logError("Moving ant to a place where there is already an ant!");;
    #endif

    setAntAt(locMoveTo, movingAnt);
    setAntAt(loc, NULL);

    movingAnt->onMove(locMoveTo);

    cout << "o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction] << endl;
};

//returns the euclidean distance between two locations with the edges wrapped
double State::distance(const Location &loc1, const Location &loc2)
{
    int d1 = abs(loc1.row-loc2.row),
        d2 = abs(loc1.col-loc2.col),
        dr = min(d1, rows-d1),
        dc = min(d2, cols-d2);
    return sqrt(dr*dr + dc*dc);
};

//returns the new location from moving in a given direction with the edges wrapped
Location State::getLocation(const Location &loc, int direction)
{
    return Location( (loc.row + DIRECTIONS[direction][0] + rows) % rows,
                     (loc.col + DIRECTIONS[direction][1] + cols) % cols );
};

ostream& operator<<(ostream &os, const State &state)
{
    os << "<br>";
    for(int row=0; row<state.rows; row++)
    {
        for(int col=0; col<state.cols; col++)
            os << state.grid[row][col];
        os << "<br>"; // removed endl
    }

    return os;
};

istream& operator>>(istream &is, State &state)
{
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while(is >> inputType)
    {
        if(inputType == "end")
        {
            #ifdef __DEBUG
            state.logger.debugLog << "GAME END (errors: " << (int)state.errors << ", warnings: " << (int)state.warnings << ", structural ants: " << (int)state.structuralAnts.size() << ")" << endl;
            #endif
            state.gameover = 1;
            break;
        }
        else if(inputType == "turn")
        {
            is >> state.turn;
            break;
        }
        else //unknown line
            getline(is, junk);
    }

    if(state.turn == 0)
    {
        //reads game parameters
        while(is >> inputType)
        {
            if(inputType == "loadtime")
                is >> state.loadtime;
            else if(inputType == "turntime")
                is >> state.turntime;
            else if(inputType == "rows")
                is >> state.rows;
            else if(inputType == "cols")
                is >> state.cols;
            else if(inputType == "turns")
                is >> state.turns;
            else if(inputType == "viewradius2")
            {
                is >> state.viewradius;
                state.viewradius = sqrt(state.viewradius);
            }
            else if(inputType == "attackradius2")
            {
                is >> state.attackradius;
                state.attackradius = sqrt(state.attackradius);
            }
            else if(inputType == "spawnradius2")
            {
                is >> state.spawnradius;
                state.spawnradius = sqrt(state.spawnradius);
            }
            else if(inputType == "ready") //end of parameter input
            {
                state.setup();
                break;
            }
            else    //unknown line
                getline(is, junk);
        }
    }
    else
    {
        //reads information about the current turn
        while(is >> inputType)
        {
            if(inputType == "w") //water square
            {
                is >> row >> col;
                state.grid[row][col] = '%';
            }
            else if(inputType == "f") //food square
            {
                is >> row >> col;
                state.grid[row][col] = '*';
            }
            else if(inputType == "a") //live ant square
            {
                is >> row >> col >> player;
                state.grid[row][col] = 'a' + player;
                if(player == 0) {
                    Location loc = Location(row, col);
                    state.ants.push_back(Location(row, col));
                    if (!state.getAntAt(loc)) state.setAntAt(loc, new Ant(state, loc));
                }
            }
            else if(inputType == "d") { //dead ant square
                is >> row >> col >> player;
                state.grid[row][col] = 'd';

            }
            else if(inputType == "players") //player information
                is >> state.players;
            else if(inputType == "scores") //score information
            {
                state.scores = vector<double>(state.players, 0.0);
                for(int p=0; p<state.players; p++)
                    is >> state.scores[p];
            }
            else if(inputType == "go") //end of turn input
            {
                if(state.gameover)
                    is.setstate(std::ios::failbit);
                else
                    state.timer.start();
                break;
            }
            else //unknown line
                getline(is, junk);
        }
    }

    return is;
};
