
#include "heads/inputdata.h"

inline
int STREAM_FAIL(const char *FROM_FILE, int AT_LINE, const char *IN_FUNCTION)
{printf("STREAM_FAILURE, line %d, function %s, file %s\n", AT_LINE, IN_FUNCTION, FROM_FILE);return EXIT_FAILURE;};

inline
int MEMO_FAIL(const char *FROM_FILE, int AT_LINE, const char *IN_FUNCTION)
{printf("MEMO_ALLOC_FAILURE, line %d, function %s, file %s\n", AT_LINE, IN_FUNCTION, FROM_FILE);return EXIT_FAILURE;};


namespace inputdata {

	int max_uavs_avail;
    int nbr_grnds;
    int dim=2;
    double** grnds;
    double uavs_range;

    // limit of map
    double bound_1;
    double bound_2;

};


void readData(char** argv)
{
	FILE* fp;

	// read data (coordinates ground nodes)
	fp=fopen(argv[1],"r");

	// read number of available uavs
	if( fscanf(fp,"%d", &inputdata::max_uavs_avail) < 0 ){STREAM_FAIL(__FILE__, __LINE__, __FUNCTION__);}

	// read range of uavs
	if( fscanf(fp,"%lf", &inputdata::uavs_range) < 0 ){STREAM_FAIL(__FILE__, __LINE__, __FUNCTION__);}

	int i=0, j=0;
/*
	// read range of each of them
	for(i=1;i<=max_uavs_avail;i++)
		fscanf(fp,"%lf", &uavs_ranges[i]);
*/

	// read number of ground nodes and their coordinates
	if( fscanf(fp,"%d", &inputdata::nbr_grnds) < 0 ){STREAM_FAIL(__FILE__, __LINE__, __FUNCTION__);}

	// read limits of map
	if( fscanf(fp,"%lf,%lf", &inputdata::bound_1, &inputdata::bound_2) < 0 ){STREAM_FAIL(__FILE__, __LINE__, __FUNCTION__);}

	// allocate memory for ground nodes
	inputdata::grnds=(double**)malloc(inputdata::nbr_grnds*sizeof(double*));
	if(inputdata::grnds==NULL){ /* memory allocation failure */ MEMO_FAIL(__FILE__, __LINE__, __FUNCTION__); }

	for(i=0;i<inputdata::nbr_grnds;i++)
	{
		inputdata::grnds[i]=(double*)calloc(inputdata::dim,sizeof(double));// Unique, defines a point : for either a ground node or a uav
		for (j=0;j<inputdata::dim;j++)
		{
			// skip comma missing for last dim
			if(j==inputdata::dim-1)	fscanf(fp,"%lf", &inputdata::grnds[i][j]);
			else fscanf(fp,"%lf,", &inputdata::grnds[i][j]);
		}
	}

	fclose(fp);

};