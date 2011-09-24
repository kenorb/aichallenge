#ifndef RELATIVELOCATION_H_INCLUDED
#define RELATIVELOCATION_H_INCLUDED

struct relativeLocation
{
    relativeLocation() { row = col = 0; };
    relativeLocation(int r, int c) { row = r; col = c; };
    relativeLocation(int r, int c, double d) { row = r; col = c; distance = d; };

    int row, col;
    double distance;
};

#endif // RELATIVELOCATION_H_INCLUDED
