#ifndef CONST_H_INCLUDED
#define CONST_H_INCLUDED

const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}
const int DIRECTION[4] = {0, 1, 2, 3};

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#endif // CONST_H_INCLUDED
