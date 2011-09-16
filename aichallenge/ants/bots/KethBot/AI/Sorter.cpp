#include "Sorter.h"
#include "globals.h"
#include "Location.h"
#include "Map.h"
#include "Optimizer.h"

bool SortFunction(int a, int b, int c, int d, int e, int f, bool symetric)
{
    double d1;
    double d2;
    if (symetric) {
        d1 = distance_fast(a, b, e, f);
        d2 = distance_fast(c, d, e, f);
    } else {
        d1 = distance_real(a, b, e, f);
        d2 = distance_real(c, d, e, f);
    }
    if (d1 == d2) {
        if (a == c) {
            return b < d;
        }
        return a < c;
    }
    return d1 < d2;
}


