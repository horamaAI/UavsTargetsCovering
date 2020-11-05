

//#include "linear_solver.hpp"
//#include <stdlib>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <tuple>
#include <memory>

#include "heads/clustering.h"


using namespace std;


int main(int argc, const char* argv[])
{

	Debug::init(argc, argv); // optional way of initializing module
	
	if(argc<=1){
		printf("Please give required number of arguments\n");
		return 1;
	}

	clock_t begin = clock();

	/* !!! igraph : turn on attribute handling  Ie. even if you don't manipulate attributes explicitly, but create a graph that might have some attributes, eg. read a graph a GraphML file, you need this call before, otherwise the attributes are dropped. */
	igraph_i_set_attribute_table(&igraph_cattribute_table);

	readData(argv);

//	double radius=(uavs_range/2);
	double range=inputdata::uavs_range;
	double lb=1.0;
	unsigned long int n_clustering=0;
	unsigned long int n_scp=0;

	//translate(res, radius);

	vector<double*>* res = elbow();
	n_clustering=res->size();

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

	//unique_ptr<Solution> rawsln(new Solution());// create first raw solution
	Solution* rawsln(new Solution());// create first raw solution
	//rawsln->gcovs_=new vector<int>[inputdata::nbr_grnds];

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

	// linear solver returns tuples of 1. indices of uavs active for coverage, and 2. the results of their linear program values
	map<int,double>* lp_covers_solution=glpk_solve_linear_model(rawsln, range, lb);


	// store results of mip solver
	FILE* fp;
	fp=fopen("./out/activeuavs.csv","w");
	for(map<int,double>::iterator it=lp_covers_solution->begin(); it!=lp_covers_solution->end(); it++)
	{
		for (int j=0; j<inputdata::dim; j++)
		{
			// skip comma not needed after last dim value
			fprintf(fp,"%lf", rawsln->uavs_[it->first][j]);
			if(j==inputdata::dim-1)	fprintf(fp,"\n");
			else	fprintf(fp,",");
		}
	}
	fclose(fp);

	// print in file connections uavs-ground nodes
	fp=fopen("./out/uavs_grounds.csv","w");
	int* activecovers = new int[inputdata::nbr_grnds];
	for(int i=0; i<inputdata::nbr_grnds; i++)
	{
		activecovers[i]=0;
		for(map<int,double>::iterator it=lp_covers_solution->begin(); it!=lp_covers_solution->end(); it++)
		{
			int uavk=it->first;
			if( euclDistance(rawsln->uavs_[uavk], inputdata::grnds[i]) <= range )
			{
				assert(rawsln->uav_in_cover(rawsln->gcovs_[i], uavk));// security safety, otherwise would be a problem
				fprintf(fp,"%lf,%lf\n", inputdata::grnds[i][0], inputdata::grnds[i][1]);
				fprintf(fp,"%lf,%lf\n\n", rawsln->uavs_[uavk][0], rawsln->uavs_[uavk][1]);
				activecovers[i]++;
			}
		}
	}

	int max=0;
	double sum=0;
//	printf("Couples (ground, n active uavs covering it) : ");
	for(int i=0; i<inputdata::nbr_grnds; i++)
	{
//		printf(" (%d, %d) ",i,activecovers[i]);
		sum+=activecovers[i];
		if(activecovers[i] > activecovers[max])	max=i;
	}
	printf("\nAverage of distribution of provided covers on all ground nodes : %f\n",sum/inputdata::nbr_grnds);
	printf("Max degree : %d with %d\n", max, activecovers[max]);


	// copy solution into new and cleaner one, and at the same time store in file the coordinates of active uavs for coverage
	unique_ptr<Solution> final_graph(new Solution());
	//final_graph->gcovs_=new vector<int>[inputdata::nbr_grnds];

//	FILE* fp;
//	fp=fopen("./out/finalres.csv","w");
	for(map<int,double>::iterator it=lp_covers_solution->begin(); it!=lp_covers_solution->end(); it++)
	{
		buffdouble=new double[inputdata::dim];
		for (int j=0; j<inputdata::dim; j++)
		{
			// skip comma not needed after last dim value
//			if(j==dim-1)	fprintf(fp,"%lf\n", rawsln->uavs_[it->first][j]);
//			else fprintf(fp,"%lf,", rawsln->uavs_[it->first][j]);
			
			buffdouble[j]=rawsln->uavs_[it->first][j];
		}
		final_graph->addTonetwork(buffdouble, range);// update distance of network of uavs
	}
//	fclose(fp);
	n_scp=final_graph->uavs_.size();

//	delete rawsln;
//	rawsln=nullptr;

	// used for restrictions
	stack<tuple<unsigned long int, unsigned long int>>* edges_for_restrictions
		=new stack<tuple<unsigned long int, unsigned long int>>;
	vector<int> *uavsccs=new vector<int>;// indices of uavs used to link sparse connected components, empty at start
	igraph_t* solG0=nullptr;

//	populate(net, uavscoverage, &uavsccs, range, solG0, edges_for_restrictions);
	final_graph->populate(uavsccs, range, solG0, edges_for_restrictions);

cerr<<"Of initial number of generated positions :"<<n_clustering<<endl;
cerr<<"number of activated uavs before populating :"<<lp_covers_solution->size()<<endl;
cerr<<", and thus number of additional uavs = "<<final_graph->uavs_.size()-lp_covers_solution->size()<<endl;
sum=0;
for(int i=0; i<inputdata::nbr_grnds; i++)
sum+=final_graph->gcovs_[i].size();
cerr<<"Final value of sum of each gcovs_[x]->size() = "<<sum<<endl;

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("|Time spent %f\n",time_spent);

//	Store the coordinates of uavs used for connectivity
	fp=fopen("./out/uavs_connectivity.csv","w");

	for(unsigned long int i=n_scp; i<final_graph->uavs_.size(); i++)
	{
		for (int j=0; j<inputdata::dim; j++)
		{
			// skip comma not needed after last dim value
			if(j==inputdata::dim-1)	fprintf(fp,"%lf\n", final_graph->uavs_[i][j]);
			else fprintf(fp,"%lf,", final_graph->uavs_[i][j]);
		}
	}
	fclose(fp);

	// write into file to save memory
	char path[30];
	char buff[30];
	strcpy(path,"./out/runtime_growth/");
	sprintf(buff, "%d_targets.csv", inputdata::nbr_grnds);
	strcat(path,buff);
	fp=fopen(path,"a");

	//fp=fopen("./out/runtime_growth/runtime.csv","a");
	fprintf(fp, "%d, %f, %lu, %lu, %lu, %f, %f\n",
		inputdata::nbr_grnds, lb, n_clustering, n_scp, final_graph->uavs_.size(), sum, time_spent);
	fclose(fp);

cout<<"THE END"<<endl;

	return 0;
}
