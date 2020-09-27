

//#include "linear_solver.hpp"
//#include <stdlib>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <tuple>
#include <memory>

#include "heads/clustering.h"


using namespace std;


int main(int argc, char** argv)
{

	clock_t begin = clock();

	/* !!! igraph : turn on attribute handling  Ie. even if you don't manipulate attributes explicitly, but create a graph that might have some attributes, eg. read a graph a GraphML file, you need this call before, otherwise the attributes are dropped. */
	igraph_i_set_attribute_table(&igraph_cattribute_table);

	readData(argv);

//	double radius=(uavs_range/2);
	double range=inputdata::uavs_range;

	//translate(res, radius);

	vector<double*>* res = elbow();

printf("In main res size : %lu\n", res->size());
/*
	i--;// out print where it stopped
	printf("\nFinal series %d, wss : %f, res uavs %d\n\n", i, wss, res->uavs_.size());
*/
/*
	FILE* fp;
	fp=fopen("./out/resTrue.csv","w");
	for(i=1;i<=res->n_uavs;i++)
		for (j=0;j<dim;j++)
		{
			// skip comma not needed after last dim value
			if(j==dim-1)	fprintf(fp,"%lf\n", res->uavs_[i][j]);
			else fprintf(fp,"%lf,", res->uavs_[i][j]);
		}
	fclose(fp);
*/

	unique_ptr<Solution> rawsln(new Solution());// create first raw solution
	rawsln->gcovs_=new vector<int>[inputdata::nbr_grnds];

	double* buffdouble;
	for(unsigned long int i=0; i<res->size(); i++){
		buffdouble=new double[inputdata::dim];
		for(int j=0; j<inputdata::dim; j++)	buffdouble[j]=(*res)[i][j];
		rawsln->addTonetwork(buffdouble, range);
	}

	// housekeeping
	for(unsigned long int j=0;j<res->size();j++)
	{
		delete[] (*res)[j];
		(*res)[j]=nullptr;
	}
	delete res;//Housekeeping
	res=nullptr;

	double lb=2.0;


	// linear solver : return indices of uavs active for coverage, and the results of their linear program values
	map<int,double>* uavscoverage=rawsln->solve_linear_model(range, lb);

	// copy solution into new and cleaner one, and at the same time store in file the coordinates of active uavs for coverage
	unique_ptr<Solution> net(new Solution());
	net->gcovs_=new vector<int>[inputdata::nbr_grnds];

//	FILE* fp;
//	fp=fopen("./out/finalres.csv","w");
	for(map<int,double>::iterator it=(*uavscoverage).begin(); it!=(*uavscoverage).end(); it++)
	{
		buffdouble=new double[inputdata::dim];
		for (int j=0; j<inputdata::dim; j++)
		{
			// skip comma not needed after last dim value
//			if(j==dim-1)	fprintf(fp,"%lf\n", rawsln->uavs_[it->first][j]);
//			else fprintf(fp,"%lf,", rawsln->uavs_[it->first][j]);
			
			buffdouble[j]=rawsln->uavs_[it->first][j];
		}
		net->addTonetwork(buffdouble, range);// update distance of network of uavs
	}
//	fclose(fp);
	
//	delete rawsln;
	rawsln=nullptr;

	unique_ptr<stack<tuple<int,int>>> pairs(new stack<tuple<int,int>>);// used for restrictions
	vector<int> uavsccs;// indices of uavs used to link sparse connected components, empty at start
	igraph_t* solG0=nullptr;

//	populate(net, uavscoverage, &uavsccs, range, solG0, pairs);
	net->populate(&uavsccs, range, solG0, pairs);


	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("|Time spent %f\n",time_spent);

//	Store the coordinates of the active uavs
//	FILE* fp;
//	fp=fopen("./out/lin_sol1cover.csv","w");
/*
	fp=fopen("./out/lin_solRange250.csv","w");
	for(i=1;i<=res->n_uavs;i++)
	{
		if(soln[i]>0)
			for (j=0;j<dim;j++)
			{
				// skip comma not needed after last dim value
				if(j==dim-1)	fprintf(fp,"%lf\n", res->uavs_[i][j]);
				else fprintf(fp,"%lf,", res->uavs_[i][j]);
			}
	}
	fclose(fp);
*/
	return 0;
};
