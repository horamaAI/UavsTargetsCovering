#include "heads/clustering.h"

double k_means(double** data, int ngrounds, vector<double*>* clusts, double error_tolerance, double range)
{

    int h, i, j; /* loop counters, of course :) */

    double old_error, error = DBL_MAX; // DBL_MAX : macro defines maximum finite floating-point value : 1E+37
	int ninres[clusts->size()]={};// gives the n elements in each cluster
	int gcovs[inputdata::nbr_grnds];
	vector<double*> cl(clusts->size());// temporary centroids
	for(h=0; h<clusts->size(); h++)
		cl[h]=new double[inputdata::dim];

    assert(data && clusts->size() > 0 && error_tolerance >= 0); /* for debugging */

    // initialization : can start with any centroid(ordering changes the final result), here first

	double min_distance = DBL_MAX, distance = 0;

    // Kmeans loop
    do {
        // save error from last step
		old_error = error;
        error = 0;

        // clear old counts and temp centroids
        for (i=0; i<clusts->size(); i++)
        {
			// reinit temp centroids
            for (j=0; j<inputdata::dim; cl[i][j++] = 0){};
			ninres[i]=0;
        }

        for (h = 0; h < ngrounds; h++)
        {
			gcovs[h] = 0;
            // Find closest cluster
            min_distance = DBL_MAX;
            for (i=0; i<clusts->size(); i++)
            {
				distance=0;
				// considered measure : squared residuals
                for(j=0; j<inputdata::dim; j++)	distance+=pow(data[h][j]-(*clusts)[i][j],2);
                if (distance < min_distance)
                {
					min_distance=distance;// find the closest cluster
                    gcovs[h] = i;
                }
            }
			// update temp centroid of destination cluster
			for(j=0; j<inputdata::dim; j++)	cl[gcovs[h]][j] += data[h][j];
			ninres[gcovs[h]]++;
            /* update standard error */
            error += min_distance;
        }

		// update centroids
        for (i=0; i<clusts->size(); i++)
            for (j=0; j<inputdata::dim; j++)
                (*clusts)[i][j] = ninres[i] ? cl[i][j]/ninres[i] : cl[i][j];

    } while (fabs(error - old_error) > error_tolerance);/* if for each iteration, the number of changes made are not
														 different from previous */

	// housekeeping
	for(h=0; h<clusts->size(); h++)
	{
		delete[] cl[h];
		cl[h]=nullptr;
	}

	return error;
};



vector<double*>* onepassmethod(double** input_data, int nbr_grnds, double range)
{// first using one pass method, and then fine tuning with k-means

	int k=0,j=1;
	// required memory allocation for solution
	vector<double*>* res=new vector<double*>;
	vector<int> ninres;// gives the n elements in each cluster
	int gcovs[nbr_grnds];
	vector<double*> cl;// temporary centroids, contains sum of elements in cluster, to build clusters

	// Init first cluster with first element
	ninres.push_back(1);
	res->push_back(new double[inputdata::dim]);
	for(k=0; k<inputdata::dim; k++)
		(*res)[0][k]=input_data[0][k];

	cl.push_back(new double[inputdata::dim]);// same with temporary value
	for(k=0; k<inputdata::dim; k++)
		cl[0][k]=input_data[0][k];

	double largestdist=sqrt(pow(inputdata::bound_1,2)+pow(inputdata::bound_2,2));// Largest distance : extrem limits of the map
	double distance_to_closest_centroid=largestdist;
	double current_distance=0;

	// start 1e pass algorithm
	for(k=1; k<nbr_grnds; k++)
	{
		distance_to_closest_centroid=largestdist;
		for(j=0;j<res->size();j++)
		{
			current_distance=euclDistance(input_data[k],(*res)[j]);
			if ( current_distance < distance_to_closest_centroid )
			{
				distance_to_closest_centroid=current_distance;// keep current closest cluster
				gcovs[k]=j;
			}
		}

		if(distance_to_closest_centroid<range)
		{// closest centroid is within admissible range => add element to cluster
			ninres[gcovs[k]]++;
			//update centroids
			for(j=0; j<inputdata::dim; j++)
			{
				cl[gcovs[k]][j]+=input_data[k][j];
				(*res)[gcovs[k]][j]=cl[gcovs[k]][j]/ninres[gcovs[k]];
			}
		}
		else
		{// closest centroid not within admissible range => Create new cluster
			gcovs[k]=res->size();
			ninres.push_back(1);
			res->push_back(new double[inputdata::dim]);
			cl.push_back(new double[inputdata::dim]);
			for(j=0; j<inputdata::dim; j++){
				(*res)[res->size()-1][j]=input_data[k][j];
				cl[res->size()-1][j]=input_data[k][j];
			}
		}
	}
	
	// Housekeeping
	for(k=0;k<res->size();k++)
	{
		delete[] cl[k];
		cl[k]=nullptr;
	}

	return res;
};


void elbow(vector<double*>* finalres){

	vector<double*>* res;// true result (buffer)
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

printf("nbr grnds : %d\n", inputdata::nbr_grnds);

	do{
		
		// do until elbow criteria satisfied:
		res_plus_2=onepassmethod(inputdata::grnds, inputdata::nbr_grnds, inputdata::uavs_range/i);
		wss=k_means(inputdata::grnds, inputdata::nbr_grnds, res_plus_2, 0.0001, inputdata::uavs_range/i);
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
printf("Res result size : %d\n", res->size());

}