double k_means(double** data, int ngrounds, vector<double*>* clusts, double error_tolerance, double range)
{

    int h, i, j; /* loop counters, of course :) */

    double old_error, error = DBL_MAX; // DBL_MAX : macro defines maximum finite floating-point value : 1E+37
	int ninres[clusts->size()]={};// gives the n elements in each cluster
	int gcovs[nbr_grnds];
	vector<double*> cl(clusts->size());// temporary centroids
	for(h=0;h<clusts->size();h++)
		cl[h]=new double[dim];

    assert(data && clusts->size() > 0 && error_tolerance >= 0); /* for debugging */

    // initialization : can start with any centroid(ordering changes the final result), here first

	double min_distance = DBL_MAX, distance = 0;

    // Kmeans loop
    do {
        // save error from last step
		old_error = error;
        error = 0;

        // clear old counts and temp centroids
        for (i=0; i <clusts->size(); i++)
        {
			// reinit temp centroids
            for (j=0;j<dim; cl[i][j++] = 0){};
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
                for(j=0;j<dim;j++)	distance+=pow(data[h][j]-(*clusts)[i][j],2);
                if (distance < min_distance)
                {
					min_distance=distance;// find the closest cluster
                    gcovs[h] = i;
                }
            }
			// update temp centroid of destination cluster
			for(j=0;j<dim;j++)	cl[gcovs[h]][j] += data[h][j];
			ninres[gcovs[h]]++;
            /* update standard error */
            error += min_distance;
        }

		// update centroids
        for (i=0; i<clusts->size(); i++)
            for (j=0; j<dim; j++)
                (*clusts)[i][j] = ninres[i] ? cl[i][j]/ninres[i] : cl[i][j];

    } while (fabs(error - old_error) > error_tolerance);/* if for each iteration, the number of changes made are not different from previous */

	// housekeeping
	for(h=0;h<clusts->size();h++)
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
	res->push_back(new double[dim]);
	for(k=0;k<dim;k++)
		(*res)[0][k]=input_data[0][k];

	cl.push_back(new double[dim]);// same with temporary value
	for(k=0;k<dim;k++)
		cl[0][k]=input_data[0][k];

	double largestdist=sqrt(pow(bound_1,2)+pow(bound_2,2));// Largest distance : extrem limits of the map
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
			for(j=0;j<dim;j++)
			{
				cl[gcovs[k]][j]+=input_data[k][j];
				(*res)[gcovs[k]][j]=cl[gcovs[k]][j]/ninres[gcovs[k]];
			}
		}
		else
		{// closest centroid not within admissible range => Create new cluster
			gcovs[k]=res->size();
			ninres.push_back(1);
			res->push_back(new double[dim]);
			cl.push_back(new double[dim]);
			for(j=0;j<dim;j++){
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