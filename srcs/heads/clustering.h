#ifndef UAVSCOVS_HEADS_CLUSTERING_H_
#define UAVSCOVS_HEADS_CLUSTERING_H_

#include "glpk_linear_solver.h"

vector<double*>* elbow();
vector<double*>* onepassmethod(double** input_data, int nbr_grnds, double range);
//double k_means(double** data, int ngrounds, vector<double*>* clusts, double error_tolerance, double range);
double k_means(double** data, int ngrounds, vector<double*>* clusts, double error_tolerance);


#endif // UAVSCOVS_HEADS_CLUSTERING_H_