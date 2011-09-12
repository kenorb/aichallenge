#ifndef SORTER_H
#define SORTER_H

#include "globals.h"
#include "Location.h"
#include "relativeLocation.h"
#include "Optimizer.h"



bool SortFunction(int a, int b, int c, int d, int e, int f, bool symetric);

inline bool sorterLocationCompare(const relativeLocation* loc1, const relativeLocation* loc2) { return loc1->row == loc2->row && loc1->col == loc2->col; };

class sortLocationsFrom
{
    int e, f;
    bool symetricSort;

    public:
        sortLocationsFrom(const Location& fromLoc, bool symetric = true) {
            e = fromLoc.row;
            f = fromLoc.col;
            symetricSort = symetric;
        }
        sortLocationsFrom(const relativeLocation& fromLoc, bool symetric = true) {
            e = fromLoc.row;
            f = fromLoc.col;
            symetricSort = symetric;
        }

        bool operator()(const Location* o1, const Location* o2) const {
            return SortFunction(o1->row, o1->col, o2->row, o2->col, e, f, symetricSort);
        }

        bool operator()(const relativeLocation* o1, const relativeLocation* o2) const {
            return SortFunction(o1->row, o1->col, o2->row, o2->col, e, f, symetricSort);
        }
};




#endif // SORTER_H
