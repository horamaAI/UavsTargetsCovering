#!/bin/bash

#printf \
#"# n targets, lower bound scp, 1st phase n centroids, 2nd phase n coverages, final n uavs deployed, total redundancy, runtime\n" \
#> ./out/growthruntime.csv

prefix=./data/
sufix=_grounds.csv


for i in {1..3}
do
    printf \
    "# n targts, lower bound scp, phase 1 n centroids, phase 2 n coverages, final n uavs deployed, tot redundancy, runtime\n" \
    > ./out/runtime_growth/"$(($i*50))"_targets.csv
    for j in {1..20}
    do
        ./a.out $prefix"$(($i*50))"$sufix
    done
done