#ifndef UAVSCOVS_HEADS_GLPK_LINEAR_SOLVER_H_
# define UAVSCOVS_HEADS_GLPK_LINEAR_SOLVER_H_


#include "solution.h"

map<int,double>* glpk_solve_linear_model(Solution* res, double range, double lb);

#endif // UAVSCOVS_HEADS_GLPK_LINEAR_SOLVER_H_