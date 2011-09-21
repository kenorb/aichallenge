#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string.h>

#include "Map.h"
#include "Optimizer.h"
#include "Genetic.h"
#include "Sorter.h"
#include "Location.h"

#include "globals.h"
#include "relativeLocation.h"


void rmdup(u4 *array, int length)
{
    u4 *current , *end = array + length - 1;
    int i, j;
/*
   for ( current = array + 1; array < end; array++, current = array + 1 ) {
        while ( current < end )
        {
            if ( *current == *array )
            {
                std::cout << "Duplicate" << std::endl;
            }
            else
            {
                current++;
            }
        }
   }
*/

    for (i=0;i<length;i++)
    for (j=i+1;j<length;j++)
    {
        if (array[i] == array[j]) {
            std::cout << "Duplicate at index " << i << " and " << j << std::endl;
        }
    }


    std::cout << "No duplicates" << std::endl;
}

void incArea(char* area, int i)
{
    if (area[i] == '.') area[i] = 'b'; else
    if (area[i] == 'b') area[i] = 'a'; else
    if (area[i] == 'a') {
        incArea(area, i+1);
        area[i] = '.';
    }
}

void Optimizer::init() {

/*
    int len = 16;
    int combinations = (len*len*len);

    u4* hashTable;
    hashTable = new u4[100000000];
    //hashTable = new uint32_t[281474976710656];

    std::string beginArea = std::string(len, '.');
    std::string endArea = std::string(len, 'a');

    char *area;
    area = new char[len+1];
    strcpy(area, beginArea.c_str());

    std::cout << std::endl << std::endl;
    //std::cout << area << std::endl;

    int i = 0;
    int k = 0;

    bool done = false;
    while (!done) // 'aaaa'
    {
        hashTable[i] = hashArea(area, len);
        //std::cout << i << " (" << area << ") = " << hashTable[i] << std::endl;

        if (hashTable[i] == hashArea(endArea.c_str(), len)) {
            done = true;
            break;
        }

        incArea(area, 0);


        i++;


    }

    std::cout << "Combinations: " << i << std::endl;
    std::cout << "Duplicates: " << k << std::endl;
    //hashTable[i] = 3;
    //hashTable[i+1] = 3;
    std::cout << "Scanning duplicates ... " << std::endl;
    rmdup(hashTable, i);


    int a = TEST_HASHES;

    int* hashes;
    hashes = new int[TEST_HASHES];


    test[0] = 1;
    test[1] = 2;
    test[2] = 3;
    test[3] = 2;
    test[4] = 23;


    rmdup(test, 6);


    std::cout << "'" << area << "' = " << hashArea(area, 4) << std::endl;
*/
    //cin.ignore(1);
    //exit(1);



    #ifdef __OPTIMIZER_TEST
        state.rows = 128;
        state.cols = 128;
        state.attackradius = sqrt(5);
        gameMap.onInit();
    #endif

    maxRoot = (state.rows * state.cols) * 2;

    sqrt_table = new double[maxRoot];
    for (int i = 0; i < maxRoot; i++) optimizer.sqrt_table[i] = sqrt(i);

    distance_table = new double[state.rows*state.cols];


    distance_cost_table = new uint8*[state.rows*state.cols];
    for(int i = 0; i < state.rows * state.cols; ++i) {
        distance_cost_table[i] = new uint8[state.rows*state.cols];
        memset(distance_cost_table[i], 0, sizeof(uint8) * state.rows*state.cols);
    }

    for (int row=0;row<state.rows;row++) {
        for (int col=0;col<state.cols;col++) {
            Location loc1(0, 0);
            Location loc2(row, col);

            distance_table[LocToIndex(row, col)] = gameMap.distance_sqrt(loc1, loc2);
        }
    }

    for (int row = 0; row < state.rows; row++)
    for (int col = 0; col < state.cols; col++) {
        double distance = distance_real(0, 0, row, col);

        radiusAreaMap.push_back(new relativeLocation(row, col, distance));
        if (row > 0) {
            radiusAreaMap.push_back(new relativeLocation(-row, col, distance));
        }
        if (col > 0) {
            radiusAreaMap.push_back(new relativeLocation(row, -col, distance));
        }
        if (row > 0 && col > 0) {
            radiusAreaMap.push_back(new relativeLocation(-row, -col, distance));
        }
    }

    for (int r = 1; r <= sqr(OPTIMIZER_MAX_RADIUS); r++) {
        for (int row = 0; row <= r; row++)
        for (int col = 0; col <= r; col++) {
            double distance = distance_real(0, 0, row, col);

            if (distance <= sqrt(r)) {
                radiusArea[r].push_back(new relativeLocation(row, col, distance));
                if (row > 0) radiusArea[r].push_back(new relativeLocation(-row, col, distance));
                if (col > 0) radiusArea[r].push_back(new relativeLocation(row, -col, distance));
                if (row > 0 && col > 0) radiusArea[r].push_back(new relativeLocation(-row, -col, distance));
            }
        }
    }

    sort(radiusAreaMap.begin(), radiusAreaMap.end());
    radiusAreaMap.erase(unique(radiusAreaMap.begin(), radiusAreaMap.end(), sorterLocationCompare), radiusAreaMap.end());
    std::sort(radiusAreaMap.begin(), radiusAreaMap.end(), sortLocationsFrom(relativeLocation(0, 0), false));

    for (int r = 1; r <= sqr(OPTIMIZER_MAX_RADIUS); r++) {
        sort(radiusArea[r].begin(), radiusArea[r].end());
        radiusArea[r].erase(unique(radiusArea[r].begin(), radiusArea[r].end(), sorterLocationCompare), radiusArea[r].end());
        std::sort(radiusArea[r].begin(), radiusArea[r].end(), sortLocationsFrom(relativeLocation(0, 0), false));


        #if defined(__DEBUG) || defined(__OPTIMIZER_TEST)
        #endif
            #ifdef __DEBUG
            if (r == 1) logger.debugLog << "   radiusAreaMap.size() = " << radiusAreaMap.size() << std::endl;
            //logger.debugLog << "   radiusArea[" << r << "].size() = " << radiusArea[r].size() << std::endl;
            #endif
            #ifdef __OPTIMIZER_TEST
            if (r == 1) std::cout << "   radiusAreaMap.size() = " << radiusAreaMap.size() << std::endl;
            //std::cout << "   radiusArea[" << r << "].size() = " << radiusArea[r].size() << std::endl;
            #endif
        #if defined(__DEBUG) || defined(__OPTIMIZER_TEST)
        #endif
    }

    #ifdef __DEBUG
/*
    for (int i = 0; i < radiusArea[3].size(); i++) {
        logger.debugLog << "radiusArea[" << i << "] = " << radiusArea[3][i]->row << "x" << radiusArea[3][i]->col << std::endl;
    }
*/
/*
    for (int i = 0; i < radiusAreaMap.size(); i++) {
        logger.debugLog << "radiusAreaMap[" << i << "] = " << radiusAreaMap[i]->row << "x" << radiusAreaMap[i]->col << std::endl;
    }
*/
    #endif
}

Optimizer::~Optimizer() {
    delete [] sqrt_table;
    delete [] distance_table;
}

#ifdef __OPTIMIZER_TEST
void Optimizer::testOptimizer() {
    double timeStart, timeEnd;

    std::cout << "init Optimizer ... " << std::endl;
    timeStart = unix_time();

    Optimizer::init();

    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;

    std::cout << "Generating square root table of " << state.rows << "x" << state.cols << " map... " << std::endl;
    timeStart = unix_time();

    sqrt_table = new double[maxRoot];
    for (int i = 0; i < maxRoot; i++) sqrt_table[i] = sqrt(i);

    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;

    std::cout << "Start calculating 1000 x " << state.rows << "x" << state.cols << " square root... " << std::endl;
    timeStart = unix_time();
    for (int j = 0; j < 1000; j++) {
        for (int i=0; i < maxRoot; i++) {
            double a = sqrt(i);
        }
    }
    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;

    std::cout << "Start calculating 1000 x " << state.rows << "x" << state.cols << " square root using table... " << std::endl;
    timeStart = unix_time();
    for (int j = 0; j < 1000; j++) {
        for (int i=0; i < maxRoot; i++) {
            double a = optimizer.sqrt_table[i];
        }
    }
    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;

    std::cout <<state.rows << "x" << state.cols << " scan with distance_sqrt... " << std::endl;
    timeStart = unix_time();

    for (int x1=0;x1<state.rows;x1++) {
        for (int y1=0;y1<state.cols;y1++) {
            for (int x2=0;x2<state.rows;x2++) {
                for (int y2=0;y2<state.cols;y2++) {
                    Location loc1(x1, y1);
                    Location loc2(x2, y2);
                    double dist = gameMap.distance_sqrt(loc1, loc2);
                }
            }
        }
    }

    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;

    std::cout << "" << state.rows << "x" << state.cols << " scan with distance_lut... " << std::endl;
    timeStart = unix_time();

    for (int x1=0;x1<state.rows;x1++) {
        for (int y1=0;y1<state.cols;y1++) {
            for (int x2=0;x2<state.rows;x2++) {
                for (int y2=0;y2<state.cols;y2++) {
                    Location loc1(x1, y1);
                    Location loc2(x2, y2);
                    double dist = gameMap.distance_lut(loc1, loc2);
                }
            }
        }
    }
    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;


    std::cout << "128x128 scan with distance_table func... " << std::endl;
    timeStart = unix_time();

    for (int x1=0;x1<state.rows;x1++) {
        for (int y1=0;y1<state.cols;y1++) {
            for (int x2=0;x2<state.rows;x2++) {
                for (int y2=0;y2<state.cols;y2++) {
                    Location loc1(x1, y1);
                    Location loc2(x2, y2);
                    double dist = gameMap.distance_table(loc1, loc2);
                }
            }
        }
    }

    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;


    std::cout << state.rows << "x" << state.cols << " scan using distance_table directly... " << std::endl;
    timeStart = unix_time();

    for (int x1=0;x1<state.rows;x1++) {
        for (int y1=0;y1<state.cols;y1++) {
            for (int x2=0;x2<state.rows;x2++) {
                for (int y2=0;y2<state.cols;y2++) {
                    double dist = distance_table[LocToIndex(abs(x1-x2),abs(y1-y2))];
                }
            }
        }
    }

    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;

    std::cout << state.rows << "x" << state.cols << " scan using distance_fast... " << std::endl;
    timeStart = unix_time();

    for (int x1=0;x1<state.rows;x1++) {
        for (int y1=0;y1<state.cols;y1++) {
            for (int x2=0;x2<state.rows;x2++) {
                for (int y2=0;y2<state.cols;y2++) {
                    double dist = distance_fast(x1, y1, x2, y2);
                }
            }
        }
    }

    timeEnd = unix_time();
    std::cout << " > Time: " << (double)(timeEnd-timeStart) << "s" << std::endl << std::endl;


    std::cout << "CRC distances... " << std::endl;
    timeStart = unix_time();

    for (int x1=0;x1<state.rows;x1++) {
        for (int y1=0;y1<state.cols;y1++) {
            for (int x2=0;x2<state.rows;x2++) {
                for (int y2=0;y2<state.cols;y2++) {
                    Location loc1(y1, x1);
                    Location loc2(y2, y1);

                    double dist1 = gameMap.distance_sqrt(loc1, loc2);
                    double dist2 = gameMap.distance_lut(loc1, loc2);
                    double dist3 = gameMap.distance_table(loc1, loc2);
                    double dist4 = distance_fast(loc1.row, loc1.col, loc2.row, loc2.col);

                    if (dist1 != dist2) {
                        std::cout << "ERROR: distance_lud fails (" << dist1 << " != " << dist2 << ") " << LocationToString(loc1) << " -> " << LocationToString(loc2) << std::endl;
                        return;
                    }
                    if (dist1 != dist3) {
                        std::cout << "ERROR: distance_table fails (" << dist1 << " != " << dist3 << ") " << LocationToString(loc1) << " -> " << LocationToString(loc2) << std::endl;
                        return;
                    }
                    if (dist1 != dist4) {
                        std::cout << "ERROR: distance_fast fails (" << dist1 << " != " << dist4 << ") " << LocationToString(loc1) << " -> " << LocationToString(loc2) << std::endl;
                        return;
                    }
                }
            }
        }
    }

    std::cout << " > CRC successful" << std::endl << std::endl;

}
#endif
