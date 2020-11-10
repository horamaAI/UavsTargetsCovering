#!/bin/bash

#printf \
#"# n targets, lower bound scp, 1st phase n centroids, 2nd phase n coverages, final n uavs deployed, total redundancy, runtime\n" \
#> ./out/growthruntime.csv

prefix=./data/
sufix=_grounds.csv


# for all 20 instances: from 50 targets to 1000
for i in {1..20}
do
    startsentence="# n targts, min redund, phase 1 n centroids, phase 2 n coverages, final n uavs deployed, tot redundancy"
    printf "$startsentence, runtime, time_spent_1st_phase, time_spent_lp_prblm, time_spent_cc_phase\n"\
    > ./out/runtime_growth/"$(($i*50))"_targets.csv

    # repeat for one instance, needed to find minimum runtime
    for j in {1..20}
    do
        printf "\n===================================================================\n"
        printf "executing i = "$i" and j = "$j" with args "$prefix"$(($i*50))"$sufix"\n"
        printf "=====================================================================\n"
        ./a.out $prefix"$(($i*50))"$sufix || exit 1
    done

done