#ifndef UAVSCOVS_HEADS_CLUSTERING_H_
#define UAVSCOVS_HEADS_CLUSTERING_H_

vector<double*>* onepassmethod(double** input_data, int nbr_grnds, double range);
double k_means(double** data, int ngrounds, vector<double*>* clusts, double error_tolerance, double range);


#endif // UAVSCOVS_HEADS_CLUSTERING_H_