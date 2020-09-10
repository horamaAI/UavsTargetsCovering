

//#include "linear_solver.hpp"
//#include <stdlib>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <cmath>

#include <assert.h>
#include <float.h>
#include <tuple>
#include <stack>

#include "heads/clustering.h"


using namespace std;

inline
int STREAM_FAIL(const char *FROM_FILE, int AT_LINE, const char *IN_FUNCTION)
{printf("STREAM_FAILURE, line %d, function %s, file %s\n", AT_LINE, IN_FUNCTION, FROM_FILE);return EXIT_FAILURE;};

inline
int MEMO_FAIL(const char *FROM_FILE, int AT_LINE, const char *IN_FUNCTION)
{printf("MEMO_ALLOC_FAILURE, line %d, function %s, file %s\n", AT_LINE, IN_FUNCTION, FROM_FILE);return EXIT_FAILURE;};

int max_uavs_avail;
int nbr_grnds;
int dim=2;			// dimension of input vectors
double** grnds;		// All ground nodes coordinates
double uavs_range;	// all ranges of available uavs

// limit of map
double bound_1;
double bound_2;



int main(int argc, char** argv)
{

	clock_t begin = clock();

	/* !!! igraph : turn on attribute handling  Ie. even if you don't manipulate attributes explicitly, but create a graph that might have some attributes, eg. read a graph a GraphML file, you need this call before, otherwise the attributes are dropped. */
	igraph_i_set_attribute_table(&igraph_cattribute_table);

	readData(argv);

	//bound_1=1000;
	//bound_2=1000;

//	double radius=(uavs_range/2);
	double range=uavs_range;

//	sln *res=onepassmethod(grnds, nbr_grnds, radius);
//	sln *trueres=onepassmethod(grnds, nbr_grnds, radius);
	//translate(res, radius);

	vector<double*>* res;// true result
	vector<double*>* res_plus_1;// true result + 1
	vector<double*>* res_plus_2;// true result + 2 and current result
	vector<double*>* del;// pointer to result to be freed

	/* Elbow criteria : if 2 next wss are close : deviation(res+1,res+2) > 3/4*(deviation(res,res+1)) */
	int i=1,j=0;

	// to generate the covers
	double prev_deviation=0,wss_minus_1=0,wss=0;/* Caution : wss isn't one of the true result but result+2 */
	double elbow_ratio=0.7;// if new deviation less than three quarters of previous deviation
	// !!! Personal note : if ratio is too low, about 0.001, then there are issues, check if time.!!!!

	bool stop=false;

printf("nbr grnds : %d\n",nbr_grnds);

	do{
		
		// do until elbow criteria satisfied:
		res_plus_2=onepassmethod(grnds, nbr_grnds, range/i);
		wss=k_means(grnds, nbr_grnds, res_plus_2, 0.0001, range/i);
printf("i %d, wss. %f, n_uavs %d\n", i, wss, res_plus_2->size());
		i++;
		if(i<=3)
		{
			if(i==2)
			{
				prev_deviation=wss;
				res=res_plus_2;// just stores result
				continue;// and go to next ite			
			}
			prev_deviation=prev_deviation-wss;
			wss_minus_1=wss;
			res_plus_1=res_plus_2;// just stores result
			continue;// and go to next ite			
		}
//printf(", S:%f-%f=%f\n", prev_deviation, wss, prev_wss-wss);

		if(wss_minus_1-wss > elbow_ratio*prev_deviation)
		{
printf("Stopping condition holds : wss_minus_1-wss > elbow_ratio*prev_deviation :\n\ti %d, w-1. %f, wss. %f, prevd. %f, ratio. %f, dev %f, f. %d, g. res_plus_1->n_uavs : %d uavs, res->n_uavs : %d uavs\n"
	, i, wss_minus_1, wss, prev_deviation, elbow_ratio*prev_deviation, wss_minus_1-wss, wss_minus_1-wss < elbow_ratio*prev_deviation, res_plus_1->size(), res->size());

			// housekeeping
			for(j=0;j<res_plus_1->size();j++)
			{
				delete[] (*res_plus_1)[j];
				(*res_plus_1)[j]=nullptr;
			}
			delete res_plus_1;// no longer needed, as only true result "res" is returned
			res_plus_1=nullptr;
			for(j=0;j<res_plus_2->size();j++)
			{
				delete[] (*res_plus_2)[j];
				(*res_plus_2)[j]=nullptr;
			}
			delete res_plus_2;// same
			res_plus_2=nullptr;
			stop=true;
		}
		else{/* keep reducing the radius */

			prev_deviation=wss_minus_1-wss;
			wss_minus_1=wss;
			del=res;
			res=res_plus_1;// store result
			res_plus_1=res_plus_2;
			// housekeeping
			for(j=0;j<del->size();j++)
			{
				delete[] (*del)[j];
				(*del)[j]=nullptr;
			}
			delete del;//Housekeeping
			del=nullptr;
		}
	}while(!stop && wss>0);/// !!!!!!!!!!!!!!!!!!!!! CAREFUL!!! CHECK STOPPING CONDITION (wss>0???)
//	}while(i<1);

/*
	i--;// out print where it stopped
	printf("\nFinal series %d, wss : %f, res uavs %d\n\n", i, wss, res->uavs.size());

/*
	FILE* fp;
	fp=fopen("resTrue.csv","w");
	for(i=1;i<=res->n_uavs;i++)
		for (j=0;j<dim;j++)
		{
			// skip comma not needed after last dim value
			if(j==dim-1)	fprintf(fp,"%lf\n", res->uavs[i][j]);
			else fprintf(fp,"%lf,", res->uavs[i][j]);
		}
	fclose(fp);
*/

	Solution* rawsln=new Solution();// create first raw solution
	rawsln->gcovs=new vector<int>[nbr_grnds];
	
	double* buffdouble;
	for(i=0;i<res->size();i++){
		buffdouble=new double[dim];
		for(j=0;j<dim;j++)	buffdouble[j]=(*res)[i][j];
		addTonetwork(rawsln, buffdouble, range);
	}

	// housekeeping
	for(j=0;j<res->size();j++)
	{
		delete[] (*res)[j];
		(*res)[j]=nullptr;
	}
	delete res;//Housekeeping
	res=nullptr;

	double lb=2.0;


	// linear solver : return indices of uavs active for coverage, and the results of their linear program values
	map<int,double>* uavscoverage=solve_linear_model(rawsln, range, lb);

	// copy solution into new and cleaner one, and at the same time store in file the coordinates of active uavs for coverage
	sln* net=new sln;
	net->gcovs=new vector<int>[nbr_grnds];

//	FILE* fp;
//	fp=fopen("finalres.csv","w");
	for(map<int,double>::iterator it=(*uavscoverage).begin(); it!=(*uavscoverage).end(); it++)
	{
		buffdouble=new double[dim];
		for (j=0;j<dim;j++)
		{
			// skip comma not needed after last dim value
//			if(j==dim-1)	fprintf(fp,"%lf\n", rawsln->uavs[it->first][j]);
//			else fprintf(fp,"%lf,", rawsln->uavs[it->first][j]);
			
			buffdouble[j]=rawsln->uavs[it->first][j];
		}
		addTonetwork(net, buffdouble, range);// update distance of network of uavs
	}
//	fclose(fp);
	
	delete rawsln;
	rawsln=nullptr;

	stack<tuple<int,int>>* pairs=new stack<tuple<int,int>>;// used for restrictions
	vector<int> uavsccs;// indices of uavs used to link sparse connected components, empty at start
	igraph_t* solG0=nullptr;

//	populate(net, uavscoverage, &uavsccs, range, solG0, pairs);
	populate(net, &uavsccs, range, solG0, pairs);


	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("|Time spent %f\n",time_spent);

//	Store the coordinates of the active uavs
//	FILE* fp;
//	fp=fopen("lin_sol1cover.csv","w");
/*
	fp=fopen("lin_solRange250.csv","w");
	for(i=1;i<=res->n_uavs;i++)
	{
		if(soln[i]>0)
			for (j=0;j<dim;j++)
			{
				// skip comma not needed after last dim value
				if(j==dim-1)	fprintf(fp,"%lf\n", res->uavs[i][j]);
				else fprintf(fp,"%lf,", res->uavs[i][j]);
			}
	}
	fclose(fp);
*/
	return 0;
};
