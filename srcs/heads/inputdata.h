#ifndef UAVSCOVS_HEADS_INPUTDATA_H_
#define UAVSCOVS_HEADS_INPUTDATA_H_


namespace inputdata
{

    extern int max_uavs_avail;
    extern int nbr_grnds;
    extern int dim=2;			// dimension of input vectors
    extern double** grnds;		// All ground nodes coordinates
    extern double uavs_range;	// all ranges of available uavs

    // limit of map
    extern double bound_1;
    extern double bound_2;

}

#endif // UAVSCOVS_HEADS_INPUTDATA_H_