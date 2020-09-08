#ifndef UAVSCOVS_HEADS_UTILS_H_
#define UAVSCOVS_HEADS_UTILS_H_


void readData(char** argv);
double euclDistance(double *node1, double *node2);
int cmpfuncXaxis(const void *el1, const void *el2);
int cmpfuncYaxis(const void *el1, const void *el2);

#endif // UAVSCOVS_HEADS_UTILS_H_