#include "heads/solution.h"

/*
Solution::Solution(){
	printf("Empty args cnstrctor");
}
*/

void Solution::addTonetwork(double *toadd, double range)
{// updates the matrix of distances to avoid constantly having to compute them

	this->uavs_.push_back(toadd);
	int i, newuav=this->uavs_.size()-1;
	double dist=0;

	// since following variables are 2d arrays, can't just push back without first completing building the array,
	// which is function of the uavs that are in the vicinity of newuav, or targets that are covered by it
	// (question: possible to do : push_back(std::vector<int>()), then this->outdeg_[i].push_back() ?)
	this->outdeg_.resize(this->outdeg_.size()+1);
	this->distuav_.resize(this->distuav_.size()+1);
	this->uavcovs_.resize(this->uavcovs_.size()+1);

	for(i=0;i<newuav;i++)
	{
		dist=euclDistance(this->uavs_[i], this->uavs_[newuav]);
		if(dist<=range){
			this->outdeg_[i].push_back(newuav);
			this->distuav_[i].push_back(dist);
		}
	}
	this->find_covers(newuav, range);// find the ground the new uav covers
}



void Solution::connect_CCs(igraph_t* Gk, double range, vector<int>* uavsconnectivity
	, stack<tuple<unsigned long int, unsigned long int>>* pairs, bool dorestriction)
{// boolean value "restrict" says wether the restriction strategy used (restrict: false, and G isn't root G0), or not.
	// If restriction not applied, then fill : **restr_list, **pairs, and * npairs.
	// Otherwise, use these lists to apply the restrictions

printf("net n_uavs %lu threshold %f and restrict %d\n", this->uavs_.size(), range, dorestriction);
//printf(" vcount %li ecount %li\n", (long int)igraph_vcount(Gk), (long int)igraph_ecount(Gk));

	igraph_integer_t ncomps=-1;// numbers of connected components
	igraph_vector_t labels, compssizes;// labels of connected components for each vertex
	double min_distance=DBL_MAX;// distance of two closest nodes but out of range (in differents connected components)
	double current_distance;// used to place a new uav
	//int buffn1=-1,buffn2=-1, n1=-1, n2=-1;//two closest but out of range nodes
	unsigned long int n1=ULONG_MAX, n2=ULONG_MAX;//two closest but out of range nodes

	// Use of a brute force branching here. (Maybe undeterministic better?)
	// brute force : create one unique connected component by iteratively branching the 2 closest connected components
	// From here, a bit of a mess, pay attention to segmentation faults, clean ASAP
	int k=0;/*, added=0;*/
	// Note : avoid doing : bool restricted=false; and restrict once only. If done so, it's possible that on the next 
	// phases restricted (n1,n2)
	// is included anyway and that we end up with a graph that was already generated. Then : always restrict (n1,n2) in new G.

	bool restricted=false;// restriction is performed once only
	do
	{
debug<< "n_uavs : " << this->uavs_.size() << " k = " << k++ << " d : " << ncomps;

		if(igraph_vector_init(&labels, 0)==IGRAPH_ENOMEM || igraph_vector_init(&compssizes, 0)==IGRAPH_ENOMEM)
			printf(" Memory issues, vector init failed %s, %d, %s \n", __FILE__, __LINE__, __FUNCTION__);

		// compute number of connected components
		igraph_clusters(Gk, &labels, &compssizes, &ncomps, IGRAPH_WEAK);

//printf("CCs Graph %d has %d connected components\n", k++, ncomps);
//printf(" vcount %li ecount %li\n", (long int)igraph_vcount(Gk), (long int)igraph_ecount(Gk));

//printf("start labels\n");
//for(i=0;i<igraph_vcount(&net->gr);i++)	printf("l[%li]:%li\t",i,(long int)VECTOR(labels)[i]);
//printf("end labels\n");

		// free first used memory since initiated again later
		igraph_vector_destroy(&compssizes);

		// if graph not a single connected component, then update matrix of distance with new uavs. Note : exists
		// isolated vertex 0
		if(ncomps>1)
		{//add new node to reach one connected component
// printf("In ncomps %d connected components\n", ncomps);
			min_distance=DBL_MAX;
			// find two closest but out of range nodes
			for (unsigned long int i=0;i<this->uavs_.size();i++)
			{
				for (unsigned long int j=i+1;j<this->uavs_.size();j++)
				{
					// Not interested in nodes in same connected component
					if ( VECTOR(labels)[i] == VECTOR(labels)[j] )	continue;
					/* Find closest clusters but out of range */
					current_distance=euclDistance(this->uavs_[i], this->uavs_[j]);
					// check only if different connected component
					if(current_distance<min_distance)
					{// keep two nodes
						if( dorestriction && !restricted && get<0>(pairs->top()) == i && get<1>(pairs->top()) == j )
						{// restricted, skip
printf("i uav2 n1 n2 pairs[0] pairs[1] : %lu %lu %lu %lu %lu %lu\n", i, j, n1, n2, get<0>(pairs->top()), get<1>(pairs->top()));
printf("tests in \"restricted\", (pairs[0] == n1), (pairs[1] == n2) : %d %d\n", get<0>(pairs->top()) == n1,
get<1>(pairs->top()) == n2);
							restricted=true;
							continue;
						}
debug<< "not restricted, and n1 = " << n1 << " n2 = " << n2 << " ncomps = " << ncomps;
						min_distance=current_distance;
						n1=i;
						n2=j;
					}
				}
			}
//long int zz=igraph_vcount(Gk);
//for(long int z=0;z<zz;z++)	cout<<VECTOR(labels)[z]<<" ";
//cout<<endl;

			// free vector of labels
			igraph_vector_destroy(&labels);

			if(n1==ULONG_MAX || n2==ULONG_MAX)
			{
				printf(" Something went wrong with graph, check since adding new uav will fail %s, %d, %s \n",
					__FILE__, __LINE__, __FUNCTION__);
				return;
			}
debug<< "use ("<< this->uavs_[n1][0] << "," << this->uavs_[n1][1]<< ") and ("<< this->uavs_[n2][0] << ","
			<< this->uavs_[n2][1]<< ")";
			// assign coordinates of new created node : middle of segment [n1,n2]
			double* buffdouble=new double[inputdata::dim];
			for (int j=0; j<inputdata::dim; j++)
				buffdouble[j]=(this->uavs_[n1][j]+this->uavs_[n2][j])/2;
			this->addTonetwork(buffdouble, range);// update distance of network of uavs

			uavsconnectivity->push_back(this->uavs_.size()-1);// keep track of uavs used for connectivity

			// add new vertex and edges to graph
			igraph_add_vertices(Gk, 1, 0);			

debug<< "Tests |V(G)| = " << (long int)igraph_vcount(Gk) << ", net->uavs_.size() = " << this->uavs_.size() << " |E(G)|"
	<< (long int)igraph_ecount(Gk);

			int lastuav=this->uavs_.size()-1;
			for (unsigned long int i=0; i<this->uavs_.size(); i++)
			{
				for (unsigned long int j=0; j<this->outdeg_[i].size(); j++){
					if(this->outdeg_[i][j]==lastuav){// add edges with new uav to graph
//debug<< "A LEAST THIS DOES WORK " <<i<<"-"<<j<< endl;
						igraph_add_edge(Gk, i, lastuav);
					}
				}
			}

			if( ! dorestriction )
			{// build first connected sln, keep track of n1 and n2 for further "potential" restrictions
				pairs->push(make_tuple(n1,n2));
			}

//			if(net->dists[net->n_uavs][n1]<threshold && solverSln[net->n_uavs] > 0 && solverSln[n1] > 0)
//			if( net->dists[net->n_uavs][n1] < threshold )
//{
//printf("(%d,%d)\n",net->n_uavs, n1);
//				igraph_add_edge(G1, net->n_uavs, n1);
//}
//			if(net->dists[net->n_uavs][n2]<threshold && solverSln[net->n_uavs] > 0 && solverSln[n2] > 0)
//			if( net->dists[net->n_uavs][n2] < threshold )
//{
//printf("(%d,%d)\n",net->n_uavs, n2);
//				igraph_add_edge(G1, net->n_uavs, n2);
//}
//printf("1. dist n_uav-n1 : %lf, thresh : %lf, nuav : %d, n1 : %d\n", net->dists[net->n_uavs][n1], threshold, net->n_uavs, n1);
//printf("2. dist n_uav-n2 : %lf, thresh : %lf, nuav : %d, n2 : %d\n", net->dists[net->n_uavs][n2], threshold, net->n_uavs, n2);
//printf(" %d and %d coords (%lf,%lf) (%lf,%lf)\n", n1, n2, net->uavs_[n1][0], net->uavs_[n1][1], net->uavs_[n2][0],
//		net->uavs_[n2][1]);
//printf("dist n1-n2 after update %d - %d\n",(int)net->dists[n2][n1],(int)net->dists[n1][n2]);

//printf("with %li vertices, and new uav %d coords (%lf,%lf)\n", (long int)igraph_vcount(&net->gr), net->n_uavs,
//		net->uavs_[net->n_uavs][0], net->uavs_[net->n_uavs][1]);
//printf(" vcount %li ecount %li\n", (long int)igraph_vcount(&net->gr), (long int)igraph_ecount(&net->gr));
		}

//	}while(ncomps>2 && k++ < 1);
	}while(ncomps>1);

//printf("final (%f,%f)\n", net->uavs_[net->n_uavs][0], net->uavs_[net->n_uavs][1]);

/*
	FILE* fout;
	fout=fopen("./out/test_graph.csv","w");
	igraph_vector_t ite_edgs_first_graph;
	igraph_vector_init(&ite_edgs_first_graph, 0);
	igraph_get_edgelist(Gk, &ite_edgs_first_graph, 0);
	int nfirst=igraph_ecount(Gk);
	long int ind1first,ind2first;// needed for the cast
	for (i=0, j=0; j<nfirst; i+=2, j++)
	{
		ind1first=VECTOR(ite_edgs_first_graph)[i];
		ind2first=VECTOR(ite_edgs_first_graph)[i+1];
		fprintf(fout,"%lf,%lf\n", net->uavs_[ind1first][0], net->uavs_[ind1first][1]);
		fprintf(fout,"%lf,%lf\n", net->uavs_[ind2first][0], net->uavs_[ind2first][1]);
		fprintf(fout,"\n");
//printf("(%ld,%ld)\t", ind1first, ind2first);
//fprintf(bufffp,"%ld-%ld:%lf,%lf:%lf,%lf:W:%lf\n", buff1, buff2, net->uavs_[buff1][0], net->uavs_[buff1][1],
//		net->uavs_[buff2][0], net->uavs_[buff2][1],weight);
	}
//printf("\n");
//printf("Tests vcount %li ecount %li, nuavs %d\n", (long int)igraph_vcount(Gk), (long int)igraph_ecount(Gk), net->n_uavs);
	fclose(fout);
	igraph_vector_destroy(&ite_edgs_first_graph);
*/

}



void Solution::duplicate_uavs(double lb, int grndi, double range)
{/* lb for knowing if ground needs more covers, counts[] the list which gives for each ground node the number of uavs covering it,
	covers[] the list of aforementioned covers */

	//int j=0, n=0, newuavs=0, uavj=0, randindex=0;
	int j=0, uavj=0, randindex=0;
	double *buff;
cout<<"Duplicate uav for node "<<grndi<<endl;
	do
	{
		// duplicate one of the uavs covering ground node i
		randindex=rand()%this->gcovs_[grndi].size();
		uavj=this->gcovs_[grndi][randindex];
cout<<"Selected "<<this->uavs_[uavj][0]<<","<<this->uavs_[uavj][1]<<endl;
		buff=new double[inputdata::dim];
		for(j=0; j<inputdata::dim; j++)
			buff[j]=this->uavs_[uavj][j];
		this->addTonetwork(buff, range);// update distance of network of uavs, does also call to 'find_covers'
	/* Keep duplicating until constraint on lb is satisfied */
	}while(this->gcovs_[grndi].size() < lb);

/*
printf("Duplicated : ");
for(int a:net->gcovs_[grndi])
printf(" %d ", a);
printf("\n");
*/
}



void Solution::find_covers(int uavj, double range)
{/* Find every covers for each ground node */
		
	for(int i=0; i<inputdata::nbr_grnds; i++)
	{
//		if( euclDistance(this->uavs_[uavj],grnds[i]) <= range && !uav_in_cover(this->gcovs_[i], uavj))
		if( euclDistance(this->uavs_[uavj], inputdata::grnds[i]) <= range )
		{
if(i==63) cout << " uav: " << uavj << " - " << this->uavs_[uavj][0] << "," << this->uavs_[uavj][1] << endl;
			this->gcovs_[i].push_back(uavj);
			this->uavcovs_[uavj].push_back(i);
		}
	}
}



void Solution::populate(vector<int>* uavsconnectivity, double range, igraph_t* solnG0
	, stack<tuple<unsigned long int, unsigned long int>>* pairs)
{// note : computing diameter of graph (O(n*|E|)) can cost more than comparing each nodes (O(n^2)) : eg : Complete
 // graph : n*|E| = n*sum_i_in{1...n-1}(i) > O(n^2) for n>3

//	long int k=0;

	/* the goal of the step is to generate the best NINDIVS by unpiling (Start with last) the list of edges added to the first
	 * graph (solnsGraphs[0]) in order to create the unique connected component. The restriction on the current level of edges
	 * to unpile is given by k (if can't use c++ stl's 'stack'). The number of vertices to start with on each iteration
	 * is n_in_first_graph-k-1, where n_in_first_graph is the number of vertices in the first connected graph. */
	// long int n_in_first_graph=(long int)igraph_vcount(solnsGraphs[0]);

	// Use of a brute force branching here. Maybe undeterministic could work better.

	// 1st phase : build the 1st connected graph with igraph specs (root). Note : probably not unique connected graph
	solnG0=new igraph_t;// first connected graph to build.


	// start building graph
	if(igraph_empty(solnG0, this->uavs_.size(), IGRAPH_UNDIRECTED)==IGRAPH_EINVAL)
		printf(" Invalid number of vertices, graph init failed %s, %d, %s \n", __FILE__, __LINE__, __FUNCTION__);

printf("G0 with %lu active uavs\n", this->uavs_.size());
	// add edges to graph : two uavs in each other range
/*
int count=0;
	for(map<int,double>::iterator it=uavscoverage->begin(); it!=ab.end(); it++)
		cout << it->first << " --- " << it->second << endl;
*/
int count=0;
	for(unsigned long int i=0; i<this->uavs_.size(); i++)
	{
		for(unsigned long int j=0;j<this->outdeg_[i].size();j++)
		{// for each uav, if connected to another one, then create link in graph
			count++;
			igraph_add_edge(solnG0, i, this->outdeg_[i][j]);// since j = i+1 to nActivUAVs, then no loop
		}
	}

printf("\nend 1st graph and count %d\n",count);
printf("Before connectCCs vcount %li ecount %li and n_uavs %lu\n", (long int)igraph_vcount(solnG0),
(long int)igraph_ecount(solnG0), this->uavs_.size());

	this->connect_CCs(solnG0, range, uavsconnectivity, pairs, false);

printf("Test npairs == %lu, %lu uavs, and should be one connected component (passed connectCCs function)\n", pairs->size(),
this->uavs_.size());

	// Now that more variables are assigned => launch true populating phase
	// solnsGraphs=(igraph_t**)malloc((*npairs)*sizeof(igraph_t*));
//	igraph_t* Gk;
//	Gk=nullptr;
	igraph_vector_t edges;
	int nfirst=0;
	long int ind1,ind2;// needed for the cast
	//Solution* buffnets;
	char path[30];
	//char buff[30];
	FILE *fout;

	// On each iteration, record all data on files to save space
	strcpy(path,"./out/graphs/G_0");
	fout=fopen(path,"w");
	if(fout==NULL)
		printf("DANGER ZONNNNNNEEEEEE!!!!\n");
	igraph_vector_init(&edges, 0);
	igraph_get_edgelist(solnG0, &edges, 0);
	nfirst=igraph_ecount(solnG0);
	for (int i=0, j=0; j<nfirst; i+=2, j++)
	{
		ind1=VECTOR(edges)[i];
		ind2=VECTOR(edges)[i+1];
		fprintf(fout,"%lf,%lf\n", this->uavs_[ind1][0], this->uavs_[ind1][1]);
		fprintf(fout,"%lf,%lf\n", this->uavs_[ind2][0], this->uavs_[ind2][1]);
		fprintf(fout,"\n");
//	printf("(%ld,%ld)\t", ind1, ind2);
//fprintf(bufffp,"%ld-%ld:%lf,%lf:%lf,%lf:W:%lf\n", buff1, buff2, net->uavs_[buff1][0], net->uavs_[buff1][1],
//		net->uavs_[buff2][0], net->uavs_[buff2][1],weight);
	}
//	printf("\n");
	fclose(fout);
	igraph_vector_destroy(&edges);
	
	strcat(path,"_coords");
	fout=fopen(path,"w");
	for(unsigned long int i=0; i<this->uavs_.size(); i++)
		for (int j=0; j<inputdata::dim; j++)
		{
			// skip comma not needed after last dim value
			if(j==inputdata::dim-1)	fprintf(fout,"%lf\n", this->uavs_[i][j]);
			else fprintf(fout,"%lf,", this->uavs_[i][j]);
		}
	fclose(fout);

// ________________________________________________________________
/* uncomment here for populating steps
// ________________________________________________________________

TO ERASE : Done in addTonetwork		
		find_covers(covers, threshold);
		
int overallredundancy=0;
// Compute redundancy of G_0
for(i=1;i<=nbr_grnds;i++)	overallredundancy+=covers->g_covers[i];
printf("G_0 has overall redundancy of %d\n",overallredundancy);

		if(*npairs%2==0)
		{// we want n even number of generated files, if there are npairs of newly generated files, if added to the starting graph,
		//	we are still short of one file => easiest solution : copy starting solution in file named [blabla]_[npairs+1]
			strcpy(path,"./out/G_");
			sprintf(buff, "%d", *npairs+1);
			strcat(path,buff);
			fout=fopen(path,"w");
			igraph_vector_init(&edges, 0);
			igraph_get_edgelist(solnG0, &edges, 0);
			nfirst=igraph_ecount(solnG0);
			for (i=0, j=0; j<nfirst; i+=2, j++)
			{
				ind1=VECTOR(edges)[i];
				ind2=VECTOR(edges)[i+1];
				fprintf(fout,"%lf,%lf\n", covers->uavs_[ind1][0], covers->uavs_[ind1][1]);
				fprintf(fout,"%lf,%lf\n", covers->uavs_[ind2][0], covers->uavs_[ind2][1]);
				fprintf(fout,"\n");
	//	printf("(%ld,%ld)\t", ind1, ind2);
	//fprintf(bufffp,"%ld-%ld:%lf,%lf:%lf,%lf:W:%lf\n", buff1, buff2, net->uavs_[buff1][0], net->uavs_[buff1][1],
	//			net->uavs_[buff2][0], net->uavs_[buff2][1],weight);
			}
	//	printf("\n");
			fclose(fout);
			igraph_vector_destroy(&edges);
			
			strcat(path,"_coords");
			fout=fopen(path,"w");
			for(i=1;i<=covers->n_uavs;i++)
				for (j=0;j<dim;j++)
				{
					// skip comma not needed after last dim value
					if(j==dim-1)	fprintf(fout,"%lf\n", covers->uavs_[i][j]);
					else fprintf(fout,"%lf,", covers->uavs_[i][j]);
				}
			fclose(fout);
		}

	for (k=0; k < *npairs; k++)
	{
		buffnets=(sln*)malloc(sizeof(sln));
		// some values are not needed for filling (and allocation) since only the position of uavs (and distance between them)
		// are wanted, as well as the connections between them. Other operations (eg:what are the ground nodes covered by a
		// given uav ?) are performed in the MLMPGA
		buffnets->n_uavs=covers->n_uavs-k-1;
//printf("net->n_uavs %d and covers->n_uavs-k-1 %ld and k %ld\n", buffnets->n_uavs, covers->n_uavs-k-1, k);
		buffnets->uavs_=(double**)malloc((covers->n_uavs*5)*sizeof(double*));
		buffnets->dists=(double**)malloc((covers->n_uavs*5)*sizeof(double*));
		for(i=0;i<(covers->n_uavs*5);i++)
		{
			buffnets->uavs_[i]=(double*)calloc(dim,sizeof(double));// All uavs to origin
			buffnets->dists[i]=(double*)calloc((covers->n_uavs*5),sizeof(double));
			for(j=0;j<dim;j++)
			{
				if(i <= buffnets->n_uavs){	buffnets->uavs_[i][j]=covers->uavs_[i][j];}
				else{	buffnets->uavs_[i][j]=-1;}
			}
		}

		updateDistMat(buffnets, range);

		Gk=(igraph_t*)malloc(sizeof(igraph_t));

		if(igraph_empty(Gk, buffnets->n_uavs+1, IGRAPH_UNDIRECTED)==IGRAPH_EINVAL)
			printf("Ite %d Invalid number of vertices, graph initialisation failed %s, %d, %s \n", (int)k,
				__FILE__, __LINE__, __FUNCTION__);

//printf("Phase 1 : Edges found while comparing distances\n");
//\/\* ------------------------------- Start independent comments
//count=0;
//		for(i=1;i<=buffnets->n_uavs;i++)
//			for(j=i+1;j<=buffnets->n_uavs;j++)
//				if(buffnets->dists[i][j]<threshold)
//				{
//printf("(%ld -- %ld)\t",i,j);
//count++;
//					igraph_add_edge(Gk, i, j);// since j = i+1 to nActivUAVs, then no loop
//				}
//\*\/ ------------------------------- End independent comments
//printf("\nresult count %d\n", count);


//printf("Phase 2 : Edges found from igraph\n");
		igraph_vector_init(&edges, 0);
		igraph_get_edgelist(solnG0, &edges, 0);
		nfirst=igraph_ecount(solnG0);
count=0;
		// if an edge from graph G0 connects a vertex not in the new graph then skip
		for (i=0, j=0; j<nfirst; i+=2, j++)
		{
			if( VECTOR(edges)[i] <= buffnets->n_uavs && VECTOR(edges)[i+1] <= buffnets->n_uavs )
{
				igraph_add_edge(Gk, VECTOR(edges)[i], VECTOR(edges)[i+1]);
//printf("\n[(%ld -- %ld)\t]", (long int)VECTOR(edges)[i], (long int)VECTOR(edges)[i+1]);
}

//printf("(%ld -- %ld)\t", (long int)VECTOR(edges)[i], (long int)VECTOR(edges)[i+1]);
count++;
//printf("(%ld -- %ld)\t", (long int)VECTOR(edges)[i], (long int)VECTOR(edges)[i+1]);
		}
//printf("\n");
//printf("\ncount new results : %d\n", count);

//printf("tests values vcount %li ecount %li\n", (long int)igraph_vcount(Gk), (long int)igraph_ecount(Gk));

		igraph_vector_destroy(&edges);

printf("K ite %li\n", k);

		connect_CCs(buffnets, Gk, threshold, restr_list, pairs, npairs, true, *npairs-1-k);

		// write into file to save memory
		strcpy(path,"./out/G_");
		sprintf(buff, "%ld", k+1);
		strcat(path,buff);
		fout=fopen(path,"w");
		printf("%s\n",path);

//printf("Ite %li with %d\n", k, buffnets->n_uavs);

		igraph_vector_init(&edges, 0);
		igraph_get_edgelist(Gk, &edges, 0);
		nfirst=igraph_ecount(Gk);
		for (i=0, j=0; j<nfirst; i+=2, j++)
		{
			ind1=VECTOR(edges)[i];
			ind2=VECTOR(edges)[i+1];
			fprintf(fout,"%lf,%lf\n", buffnets->uavs_[ind1][0], buffnets->uavs_[ind1][1]);
			fprintf(fout,"%lf,%lf\n", buffnets->uavs_[ind2][0], buffnets->uavs_[ind2][1]);
			fprintf(fout,"\n");
//	printf("(%ld,%ld)\t", ind1, ind2);
//fprintf(bufffp,"%ld-%ld:%lf,%lf:%lf,%lf:W:%lf\n", buff1, buff2, net->uavs_[buff1][0], net->uavs_[buff1][1],
//			net->uavs_[buff2][0], net->uavs_[buff2][1],weight);
		}
//	printf("\n");
		fclose(fout);
		igraph_vector_destroy(&edges);
		free(Gk);
		
		strcat(path,"_coords");
		fout=fopen(path,"w");

		for(i=1;i<=buffnets->n_uavs;i++)
			for (j=0;j<dim;j++)
			{
				// skip comma not needed after last dim value
				if(j==dim-1)	fprintf(fout,"%lf\n", buffnets->uavs_[i][j]);
				else fprintf(fout,"%lf,", buffnets->uavs_[i][j]);
			}
		fclose(fout);

		// Housekeeping
		for(i=0;i<(covers->n_uavs*5);i++)
		{
			//for(j=0;j<dim;j++)	free(buffnets[k]->uavs_[i][j]);
			free(buffnets->uavs_[i]);
			free(buffnets->dists[i]);
		}
		free(buffnets->uavs_);
		free(buffnets->dists);
		free(buffnets);

	}

// ________________________________________________________________
\/\* END uncomment here for populating steps
// ________________________________________________________________



// ------------------------------------------------------    note to myself : start Independent comments
\/\* 
if(++k==1)
{// keep topology of initial graph
	FILE* fout;
	fout=fopen("./out/first_graph.csv","w");
	igraph_vector_t ite_edgs_first_graph;
	igraph_vector_init(&ite_edgs_first_graph, 0);
	igraph_get_edgelist(&G1, &ite_edgs_first_graph, 0);
	int nfirst=igraph_ecount(&G1);
	long int ind1first,ind2first;// needed for the cast
	for (i=0, j=0; j<nfirst; i+=2, j++)
	{
		ind1first=VECTOR(ite_edgs_first_graph)[i];
		ind2first=VECTOR(ite_edgs_first_graph)[i+1];
		fprintf(fout,"%lf,%lf\n", net->uavs_[ind1first][0], net->uavs_[ind1first][1]);
		fprintf(fout,"%lf,%lf\n", net->uavs_[ind2first][0], net->uavs_[ind2first][1]);
		fprintf(fout,"\n");
printf("(%ld,%ld)\t", ind1first, ind2first);
//fprintf(bufffp,"%ld-%ld:%lf,%lf:%lf,%lf:W:%lf\n", buff1, buff2, net->uavs_[buff1][0], net->uavs_[buff1][1],
//		net->uavs_[buff2][0], net->uavs_[buff2][1],weight);
	}
printf("\n");
	fclose(fout);
	igraph_vector_destroy(&ite_edgs_first_graph);
}


	// write graph into file
	FILE* fp;
	fp=fopen("./out/genet_clust.csv","w");
	igraph_vector_t ite_edgs;
	igraph_vector_init(&ite_edgs, 0);
	igraph_get_edgelist(&G1, &ite_edgs, 0);
	int n=igraph_ecount(&G1);
	long int ind1,ind2;// needed for the cast
	for (i=0, j=0; j<n; i+=2, j++)
	{
		ind1=VECTOR(ite_edgs)[i];
		ind2=VECTOR(ite_edgs)[i+1];
		fprintf(fp,"%lf,%lf\n", net->uavs_[ind1][0], net->uavs_[ind1][1]);
		fprintf(fp,"%lf,%lf\n", net->uavs_[ind2][0], net->uavs_[ind2][1]);
		fprintf(fp,"\n");
printf("(%ld,%ld)\t", ind1, ind2);
//fprintf(bufffp,"%ld-%ld:%lf,%lf:%lf,%lf:W:%lf\n", buff1, buff2, net->uavs_[buff1][0], net->uavs_[buff1][1],
//		net->uavs_[buff2][0], net->uavs_[buff2][1],weight);
	}
printf("\n");
	fclose(fp);
	igraph_vector_destroy(&ite_edgs);
*/
// ------------------------------------------------------    Note to myself: end independent comments

}



bool Solution::uav_in_cover(vector<int> &gcovs, int uavindex){
	for(unsigned long int i=0; i<gcovs.size(); i++){
		if(gcovs[i]==uavindex)
			return true;
	}
	return false;
}



void Solution::updateDistMat(double range)
{// updates the matrix of distances, reminder :
	// vector<vector<int>> outdeg;// 1. outdegree relation of uav, note: j < outdeg[j] to avoid symmetries
	// vector<vector<double>> distuav;// dists[j][outdeg[j]] = distance between uav j and uav outdeg[j]

	double dist=0;
	vector<tuple<int,int>> todel;// edges to eventually delete if new distance greater than 'range'

	for(unsigned long int i=0; i<this->uavs_.size(); i++)
	{
		for(unsigned long int j=0; j<this->outdeg_[i].size(); j++)
		{
			int adjac_uav = this->outdeg_[i][j];
			dist=euclDistance(this->uavs_[i], this->uavs_[adjac_uav]);
			if (dist>range){
				// remove edge if j and adjac_uav are too far apart, or one of the two uavs is unactive
				todel.push_back(make_tuple(i,j));
			}
			this->distuav_[i][j]=dist;// expected to exist, created with calls to 'addTonetwork' 
		}
	}

	if(!todel.empty()){
		for(unsigned long int i=0; i<todel.size(); i++){
			int buffi=get<0>(todel[i]);
			int buffj=get<1>(todel[i]);
			this->outdeg_[buffi].erase(this->outdeg_[buffi].begin()+buffj);
			this->distuav_[buffi].erase(this->distuav_[buffi].begin()+buffj);
		}
	}
}


