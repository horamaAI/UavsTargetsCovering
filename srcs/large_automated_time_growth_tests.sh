#!/bin/bash

#printf \
#"# n targets, lower bound scp, 1st phase n centroids, 2nd phase n coverages, final n uavs deployed, total redundancy, runtime\n" \
#> ./out/growthruntime.csv

beginning=./scripts/outs/
targets=("set1_1000_1000_map" "set2_1500_1500_map" "set3_2000_2000_map" "set4_2500_2500_map" "set5_3000_3000_map"
"set6_3500_3500_map")

for indata in "${targets[@]}"
do

    prefix="$beginning$indata/"
    sufix=_grounds.csv

    IFS='/'
    buff=$prefix
    read -ra ADDR <<< "$buff"
    # length of array
    len=${#ADDR[@]}
    target_folder=${ADDR[$len-1]}
    target_path=./out/runtime_growth/"$target_folder/"

    # for all 20 instances: from 50 targets to 1000
    for i in {1..20}
    do
        startsentence="# n targts, min redund, phase 1 n centroids, phase 2 n coverages, final n uavs deployed, tot redundancy"
        printf "$startsentence, runtime, time_spent_1st_phase, time_spent_lp_prblm, time_spent_cc_phase\n"\
        > "$target_path$(($i*50))"_targets.csv

        # repeat for one instance, needed to find minimum runtime
        for j in {1..20}
        do
            printf "\n===================================================================\n"
            printf "executing i = "$i" and j = "$j" with args "$prefix"$(($i*50))"$sufix"\n"
            printf "=====================================================================\n"
            ./a.out "$prefix$(($i*50))$sufix" || exit 1
        done

    done

    IFS=' '

done