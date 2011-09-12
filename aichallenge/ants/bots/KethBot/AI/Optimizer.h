#ifndef OPTIMIZER_H_INCLUDED
#define OPTIMIZER_H_INCLUDED

#include <vector>

struct relativeLocation;

const int OPTIMIZER_MAP_SIZE_MAX = 128 * 128;
const int OPTIMIZER_MAX_RADIUS = 14;


struct Optimizer {
    ~Optimizer();

    void init();

    #ifdef __OPTIMIZER_TEST
    void testOptimizer();
    #endif

    double* sqrt_table;
    double* distance_table;
    std::vector<const relativeLocation*> radiusArea[(OPTIMIZER_MAX_RADIUS*OPTIMIZER_MAX_RADIUS)+1];
    std::vector<const relativeLocation*> radiusAreaMap;

    int maxRoot;
    int maxDist;

};


#endif // OPTIMIZER_H_INCLUDED
