#ifndef UAVSCOVS_HEADS_INPUTDATA_H_
#define UAVSCOVS_HEADS_INPUTDATA_H_

#include <stdio.h>
#include <fstream>
#include <vector>
#include <igraph.h>
#include <glpk.h>
#include <map>

#include "debug.h"

using namespace std;

namespace inputdata
{

    extern int max_uavs_avail;
    extern int nbr_grnds;
    extern int dim;			// dimension of input vectors
    extern double** grnds;		// All ground nodes coordinates
    extern double uavs_range;	// all ranges of available uavs

    // limit of map
    extern double bound_1;
    extern double bound_2;

}

void readData(const char* argv[]);

#endif // UAVSCOVS_HEADS_INPUTDATA_H_
