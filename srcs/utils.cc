
#include "utils.h"

double euclDistance(double *node1, double *node2)
{
	int i=0;
	double norm=0;
	for(i=0;i<dim;i++)	norm+=pow(node1[i]-node2[i],2);
    return sqrt(norm);
};

int cmpfuncXaxis(const void *el1, const void *el2)
{
	const double *uav1 = (const double*) el1;
	const double *uav2 = (const double*) el2;
	return uav1[0]-uav2[0];
};

int cmpfuncYaxis(const void *el1, const void *el2)
{
	const double *uav1 = (const double*) el1;
	const double *uav2 = (const double*) el2;
	return uav1[1]-uav2[1];
};

