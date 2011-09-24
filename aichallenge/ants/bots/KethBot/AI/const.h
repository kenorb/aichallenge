#ifndef CONST_H_INCLUDED
#define CONST_H_INCLUDED


#define pi 3.14
#define sqr(x) (x*x)

#define UNDEFINED -1

#define NO_MOVE -1
#define MOVE_UNKNOWN -2
#define MOVE_DANAGEROUS -3

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define NEAREST_LIMIT 5
#define MAX_MAGNETS 800
#define MAX_CACHE_DEPTH 5

#define ANY 0
#define FRIEND 1
#define ENEMY 2

#define FOCUSED 1
#define BLURRED 2

#define FIRST 0
#define SECOND 1
#define THIRD 2
#define FOURTH 3
#define FIFTH 4

const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};
const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}
const int DIRECTION[4] = {0, 1, 2, 3};

const uint8 FOOD_TYPES_COUNT = 3;
const uint8 FOOD_TYPE[3] = {ANY, FOCUSED, BLURRED};

const uint8 ANT_TYPES_COUNT = 3;
const uint8 ANT_TYPE[3] = {ANY, FRIEND, ENEMY};

#endif // CONST_H_INCLUDED
