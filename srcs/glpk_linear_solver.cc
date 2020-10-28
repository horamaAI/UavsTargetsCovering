#include "heads/glpk_linear_solver.h"


map<int,double>* glpk_solve_linear_model(Solution* res, double range, double lb)
{

//	/* find covers for each ground node */ : done every time a new uav is added
//	find_covers(net, range);

//	bool changedstate=false;
	/* Find at least one uav that needs to be replicated so that a minimum cover constraint isn't violated : at least
		lb covers for each target */
	for(int i=0; i<inputdata::nbr_grnds; i++)
	{
//		/* Create duplicate if constraint not satisfied => select randomly uavs covering i to duplicate, and update covers */
		if( res->gcovs_[i].size() < lb )
		{
printf("Duplicate uavs for gnode %d, which is covered with %lu uavs \n", i, res->gcovs_[i].size());
for(unsigned long int z=0;z<res->uavs_.size();z++){
//int buffz=net->gcovs_[i][z];
cout<<res->uavs_[z][0]<<","<<res->uavs_[z][1]<<endl;
}
			res->duplicate_uavs(lb, i, range);
//			if(!changedstate)	changedstate=true;
//			break;
		}
	}
/*	
	// if uavs added in duplication phase, then search again covers with ground nodes
	if(changedstate)
	{

TO ERASE : Done in addTonetwork		
		find_covers(net, range);
	}
*/

printf("Begin building linear problem with %d ground nodes, and %lu uavs\n", inputdata::nbr_grnds, res->uavs_.size());
	/* Build linear problem */
	glp_prob *lp;
	int* ia=new int[1+(inputdata::nbr_grnds*res->uavs_.size())];
	int* ja=new int[1+(inputdata::nbr_grnds*res->uavs_.size())];
	double* ar=new double[1+(inputdata::nbr_grnds*res->uavs_.size())];
	double z=0.;
	
	lp = glp_create_prob();
	glp_set_prob_name(lp, "set cover");
	glp_set_obj_dir(lp, GLP_MIN);

	/* for row names */
	vector<char*> row_names(inputdata::nbr_grnds+1);
	for(int i=0; i<inputdata::nbr_grnds+1; i++)
		row_names[i] = new char[20];
	vector<char*> col_names(res->uavs_.size()+1);
	for(unsigned long int i=0; i<res->uavs_.size()+1; i++)
		col_names[i] = new char[20];
	char buff[20];

	glp_add_rows(lp, inputdata::nbr_grnds);
	for(int i=0; i<inputdata::nbr_grnds; i++)
	{
		sprintf(buff,"r%d",i+1);
		strcpy(row_names[i+1],buff);
		glp_set_row_name(lp, i+1, row_names[i+1]);
		glp_set_row_bnds(lp, i+1, GLP_LO, lb, 0.0);
	}

	glp_add_cols(lp, res->uavs_.size());
	for(unsigned long int j=0;j<res->uavs_.size();j++)
	{
		sprintf(buff,"x%lu",j+1);
		strcpy(col_names[j+1],buff);
		glp_set_col_name(lp, j+1, col_names[j+1]);
		glp_set_col_bnds(lp, j+1, GLP_DB, 0.0, 1.0);
		glp_set_obj_coef(lp, j+1, 1.0);
		glp_set_col_kind(lp, j+1, GLP_IV);
		//glp_set_col_kind(lp, j, GLP_BV);
		//glp_set_col_kind(lp, j, GLP_CV);
	}

printf("RANGE %f, lb %f\n",range,lb);
	int counter=1;
	for(int i=0; i<inputdata::nbr_grnds; i++)
	{
		for(unsigned long int j=0; j<res->uavs_.size(); j++)
		{
			ia[counter] = i+1;
			ja[counter] = j+1;
			ar[counter] = ( euclDistance(res->uavs_[j], inputdata::grnds[i]) <= range ? 1.0 : 0.0 );
			counter++;
		}
	}

	glp_load_matrix(lp, inputdata::nbr_grnds*res->uavs_.size(), ia, ja, ar);

/*
	int ind[net->uavs_.size()+1];
	double val[net->uavs_.size()+1];
	for(i=0;i<nbr_grnds;i++)
	{
		for(j=0;j<net->uavs_.size();j++)
		{
			ind[j+1]=0;
			val[j+1]=0;
		}
		int len=glp_get_mat_row(lp, i+1, ind, val);// stores non zero values of row 113 into "ind(indices of cols),
			val(non zero values)"
//		printf(" LO %f, COL LO %f, COL UP %f %d\n",glp_get_row_lb(lp,113),glp_get_col_lb(lp,88),glp_get_col_ub(lp,88), len);
		printf("gnode %d has %d, covers => ", i+1, len);
		for(j=1;j<=len;j++)
		printf(" %d, ", ind[j]);
		printf("\n");		
	}
*/
	
//	int result_solver=glp_simplex(lp, NULL);

	glp_iocp parm;
	glp_init_iocp(&parm);
	parm.presolve = GLP_ON;

	int result_solver2=glp_intopt(lp, &parm);
cerr<<"result_solver2 == "<<result_solver2<<endl;
//	z = glp_get_obj_val(lp);
	z = glp_mip_obj_val(lp);
	
	/* Gather results */
	map<int,double> *soln=new map<int,double>;
	// Needed for stats on covers (constraints, rows)
	double sumrows=0, maxrowval=0, minrowval=res->uavs_.size(), tmp=0, lessThanAvrg=0, moreThanvrg=0;
	double* covRes=(double*)calloc(inputdata::nbr_grnds+1,sizeof(double));

	for(int i=0; i<inputdata::nbr_grnds; i++)
	{// for statistics on the values returned by the rows
//		tmp=glp_get_row_prim(lp, i);
		tmp=glp_mip_row_val(lp, i+1);
//		printf("cstrnt %d = %f\n", i, tmp);
		sumrows += tmp;
		if(tmp > maxrowval)	maxrowval=tmp;
		if(tmp < minrowval)	minrowval=tmp;
		if(tmp >= 3){moreThanvrg++;}else{lessThanAvrg++;}
		covRes[(int)tmp]++;
	}

	for(unsigned long int j=0; j<res->uavs_.size(); j++)
	{
//		soln[j]= glp_get_col_prim(lp, j);
		double buffval=glp_mip_col_val(lp, j+1);
		if(buffval>0)
			(*soln)[j]=buffval;
//		if (soln[j] - ceil(soln[j]) != 0)	printf(" soln %d not an integer = %f\n", j, soln[j]);
	}
	
	glp_print_sol(lp, "./out/glpksols.txt");

	glp_delete_prob(lp);


//	for(map<int,double>::iterator it=(*soln).begin(); it!=(*soln).end(); it++)
//		printf("[ %d : %f ] ", it->first, it->second);
	
	printf("\nRO == %f, max = %f, min = %f, aver = %f, lessThanAvrg = %f, moreThanvrg = %f\n",sumrows, maxrowval,
		minrowval, sumrows/inputdata::nbr_grnds, lessThanAvrg, moreThanvrg);
//	for(int i=0;i<=(int)maxrowval;i++)
//        if (covRes[i]>0) printf("cov[%d]=%f\t",i,covRes[i]);
//	printf("\n");
//	int activeuavs=0;

	FILE* fp;
	fp=fopen("./out/activeuavs.csv","w");
	for(map<int,double>::iterator it=(*soln).begin(); it!=(*soln).end(); it++)
	{
		for (int j=0; j<inputdata::dim; j++)
		{
			// skip comma not needed after last dim value
			fprintf(fp,"%lf", res->uavs_[it->first][j]);
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
		for(map<int,double>::iterator it=(*soln).begin(); it!=(*soln).end(); it++)
		{
			int uavk=it->first;
			if( euclDistance(res->uavs_[uavk], inputdata::grnds[i]) <= range )
			{
				assert(res->uav_in_cover(res->gcovs_[i], uavk));// security safety, otherwise would be a problem
				fprintf(fp,"%lf,%lf\n", inputdata::grnds[i][0], inputdata::grnds[i][1]);
				fprintf(fp,"%lf,%lf\n\n", res->uavs_[uavk][0], res->uavs_[uavk][1]);
				activecovers[i]++;
			}
		}
	}

	printf(" Obj func : %f and nactive uavs %lu\n",z, (*soln).size());
	int max=0;
	double sum=0;
//	printf("Couples (ground, n active uavs covering it) : ");
	for(int i=0; i<inputdata::nbr_grnds; i++)
	{
//		printf(" (%d, %d) ",i,activecovers[i]);
		sum+=activecovers[i];
		if(activecovers[i] > activecovers[max])	max=i;
	}
	printf("\nAverage : %f\n",sum/inputdata::nbr_grnds);
	printf(" Max degree : %d with %d\n",max,activecovers[max]);
	
	// housekeeping
	delete covRes;

	return soln;
}
