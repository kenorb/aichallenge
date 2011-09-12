#include "Sorter.h"
#include "globals.h"
#include "Location.h"
#include "Map.h"
#include "Optimizer.h"

bool SortFunction(int a, int b, int c, int d, int e, int f, bool symetric)
{
    if (symetric) {
        return distance_fast(a, b, e, f) < distance_fast(c, d, e, f);
    } else {
        return distance_real(a, b, e, f) < distance_real(c, d, e, f);
    }
}


