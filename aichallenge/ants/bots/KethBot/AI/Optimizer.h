#ifndef OPTIMIZER_H_INCLUDED
#define OPTIMIZER_H_INCLUDED

#include <vector>

typedef unsigned char uint8;
typedef unsigned short uint16;

struct relativeLocation;

const int OPTIMIZER_MAP_SIZE_MAX = 128 * 128;
const int OPTIMIZER_MAX_RADIUS = 14;

struct SearchCache {
    uint16 index[2];
};

struct Optimizer {
    ~Optimizer();

    void init();
    void onNewTurn();

    #ifdef __OPTIMIZER_TEST
    void testOptimizer();
    #endif


    double* sqrt_table;
    double* distance_table;
    uint8** distance_cost_table;
    SearchCache** mapsearch_cache;
    std::vector<const relativeLocation*> radiusArea[(OPTIMIZER_MAX_RADIUS*OPTIMIZER_MAX_RADIUS)+1];
    std::vector<const relativeLocation*> radiusAreaMap;

    int maxRoot;
    int maxDist;

};


#endif // OPTIMIZER_H_INCLUDED
