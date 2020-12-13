

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

	clock_t time_overall;
	clock_t time1stphase;
	clock_t timeLPProblem;
	clock_t timeCC;

	time_overall = clock();

	/* !!! igraph : turn on attribute handling  Ie. even if you don't manipulate attributes explicitly, but create a graph that might have some attributes, eg. read a graph a GraphML file, you need this call before, otherwise the attributes are dropped. */
	igraph_i_set_attribute_table(&igraph_cattribute_table);

	readData(argv);

//	double radius=(uavs_range/2);
	double range=inputdata::uavs_range;
	double lb=1.0;
	unsigned long int n_clustering=0;
	unsigned long int n_scp=0;

	//translate(res, radius);

	time1stphase=clock();
	vector<double*>* res = elbow();
	time1stphase=clock()-time1stphase;

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

	// linear solver returns tuples of: 1. indices of uavs active for coverage, and 2. the results of their linear program values
	timeLPProblem = clock();
	map<int,double>* lp_covers_solution=glpk_solve_linear_model(rawsln, range, lb);
	timeLPProblem = clock()- timeLPProblem;

	int max=0;
	double sum=0;
	int* activecovers = new int[inputdata::nbr_grnds];

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
	//unique_ptr<Solution> final_graph(new Solution());
	Solution* final_graph(new Solution());
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
	timeCC=clock();
	final_graph->populate(uavsccs, range, solG0, edges_for_restrictions);
	timeCC=clock()-timeCC;

cerr<<"Of initial number of generated positions :"<<n_clustering<<endl;
cerr<<"number of activated uavs before populating :"<<lp_covers_solution->size()<<endl;
cerr<<", and thus number of additional uavs = "<<final_graph->uavs_.size()-lp_covers_solution->size()<<endl;
sum=0;
for(int i=0; i<inputdata::nbr_grnds; i++)
sum+=final_graph->gcovs_[i].size();
cerr<<"Final value of sum of each gcovs_[x]->size() = "<<sum<<endl;

	time_overall = clock()-time_overall;

	double time_spent_overall = (double) time_overall / CLOCKS_PER_SEC;
	double time_spent_1st_phase = (double) time1stphase / CLOCKS_PER_SEC;
	double time_spent_lp_prblm = (double) timeLPProblem / CLOCKS_PER_SEC;
	double time_spent_CC = (double) timeCC / CLOCKS_PER_SEC;
	printf("|Execution times overall :%f\n",time_spent_overall);
	printf("|- 1st clustering phase :%f\n",time_spent_1st_phase);
	printf("|- 2nd lp solver :%f\n",time_spent_lp_prblm);
	printf("|- CC phase  :%f\n",time_spent_CC);

	FILE* fp;

	// write into file to save memory
	char path[100];
	char prefix[100];
	char suffix[100];

	char target_folder[100];
	char *trgt_split=(char*)malloc(100);
	strcpy(trgt_split, argv[1]);
	strtok(trgt_split, "/");

	while(trgt_split!=NULL){
		// variable 'suffix' is used here only as a buffer
		strcpy(target_folder, suffix);
		strcpy(suffix, trgt_split);
		trgt_split=strtok(NULL, "/");
	}

	strcpy(prefix, "./out/runtime_growth/");
	strcat(prefix, target_folder);
	strcat(prefix, "/");
	sprintf(suffix, "%d_targets.csv", inputdata::nbr_grnds);
/*
cout << "VALUE TRGT FOLDER : " << target_folder << endl;
cout << " PRFIX : " << prefix << endl;
*/
	strcpy(path, prefix);
	strcat(path, suffix);
/*
cout << " SUFFIX : " << suffix << endl;
cout << " PATH:" << path << endl;
*/
	fp=fopen(path,"a");

if(fp==NULL)
cout<<"FAILED !!! "<<path<<endl;

	//fp=fopen("./out/runtime_growth/runtime.csv","a");
	fprintf(fp, "%d, %f, %lu, %lu, %lu, %f, %f, %f, %f, %f\n",
		inputdata::nbr_grnds, lb, n_clustering, n_scp, final_graph->uavs_.size(), sum
		, time_spent_overall, time_spent_1st_phase, time_spent_lp_prblm, time_spent_CC);
	fclose(fp);

	// store results of mip solver
	strcpy(path, prefix);
	strcat(path, "mip_res_");
	strcat(path, suffix);

	fp=fopen(path,"w");
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

	strcpy(path, prefix);
	strcat(path, "ccs_");
	strcat(path, suffix);

	//	Store the coordinates of uavs used for connectivity
	fp=fopen(path, "w");
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

	// print in file connections uavs-ground nodes
	strcpy(path, prefix);
	strcat(path, "uavs_grounds_");
	strcat(path, suffix);

	fp=fopen(path, "w");

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

cout<<"THE END"<<endl;

	return 0;
}
