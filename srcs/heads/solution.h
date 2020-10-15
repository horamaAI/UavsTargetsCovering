#ifndef UAVSCOVS_HEADS_SOLUTION_H_
#define UAVSCOVS_HEADS_SOLUTION_H_

#include <iostream>
#include <stack>
#include <cfloat>
#include <cstring>
#include <climits>
#include <cassert>

#include "utils.h"

class Solution{

    public :
        vector<double*> uavs_;// coordinates of uavs (vector of pointers to arrays)
        vector<int> *gcovs_;// for each ground node, the indices of uavs covering it
        vector<vector<int>> uavcovs_;// for each uav, the ground node it covers: inverse of gcovs

        // adjacency list of distances between uavs. Two structs:
        vector<vector<int>> outdeg_;// 1. outdegree relations of uav. Note: uav j < uav outdeg[j] to avoid symmetries
        vector<vector<double>> distuav_;// 2. dists[j][outdeg[j]] = distance between uav j and uav outdeg[j]
        //igraph_t* gr_;// graph of the solution. Needed for connectivity checking

    public :

        // Rule of five (Tocomplete)
        // Solution():gcovs_(nullptr){};
        Solution():uavs_(), gcovs_(nullptr), uavcovs_(), outdeg_(), distuav_(){};
        ~Solution(){
            // delete gcovs_;
            cout<<"Destructor"<<endl;
        };
//      Copy constructor: Solution(const Solution& other):gcovs_(other->gcovs_){};
        Solution(const Solution& other):uavs_(), gcovs_(nullptr), uavcovs_(), outdeg_(), distuav_(){// copy constructor
            cout<< "Copy constructor of " << &other << endl;
        };
        Solution(const Solution&& other):uavs_(), gcovs_(nullptr), uavcovs_(), outdeg_(), distuav_(){// move constructor
            cout<< "Move constructor of " << &other << endl;
        };
        Solution& operator=(const Solution& other){
            cout<< "Assignment operator override of " << &other << endl;
            if(this != &other) {
                //delete[] cstring;  // deallocate
                // init(other.cstring);
            }
            return *this;            
        };
        Solution& operator=(Solution&& other) noexcept{// move assignment
            cout<< "Move assignment of " << &other << endl;
            if(this != &other) {
                //delete[] cstring;  // deallocate
                // init(other.cstring);
            }
            return *this;            
        };


        void addTonetwork(double *toadd, double range);
        void connect_CCs(igraph_t* Gk, double range, vector<int>* uavsconnectivity
            , stack<tuple<unsigned long int, unsigned long int>>* pairs, bool dorestriction);
        void duplicate_uavs(double lb, int grndi, double range);
        void find_covers(int uavj, double range);// Find every covers of each ground node
        void populate(vector<int>* uavsconnectivity, double range, igraph_t* solnG0
            , stack<tuple<unsigned long int, unsigned long int>>* pairs);
        map<int,double>* solve_linear_model(double range, double lb);
        igraph_t* translate(double threshold, double* solverSln);
        bool uav_in_cover(vector<int> &gcovs, int uavindex);
        void updateDistMat(double range);

};


#endif // UAVSCOVS_HEADS_SOLUTION_H_
