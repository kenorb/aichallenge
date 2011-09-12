#define __OPTIMIZER

#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

#include <limits>
#include "vector2d.h"
#include <vector>
#include <sys/time.h>
#include "Sorter.h"

#ifdef __DEBUG
#include "Profiler.h"
#endif

#include "Optimizer.h"

#define __abs(x) (x<0?-x:x);

using namespace std;

struct Bot;
extern Bot bot;

struct Map;
extern Map gameMap;

#ifdef __DEBUG
struct Logger;
extern Logger logger;
#endif

struct State;
extern State state;

#ifdef __DEBUG
extern Profiler profiler;
#endif

extern Optimizer optimizer;

struct Location;
struct Food;

extern Location* locNull;
extern int codeDepth;

typedef vector<Location> locationList;
typedef vector<Food> foodList;


inline int sign(int a) { return (a == 0) ? 0 : (a < 0 ? -1 : 1); }
double unix_time();
int IndexToRow(const int row);
int IndexToCol(const int col);
//int LocToIndex(const int row, const int col);
int isPowerOfTwo(unsigned int x);

#endif // GLOBALS_H_INCLUDED
