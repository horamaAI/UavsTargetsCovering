#!/bin/bash

printf \
"# n targets, lower bound scp, 1st phase n centroids, 2nd phase n coverages, final n uavs deployed, total redundancy, runtime\n" \
> ./out/growthruntime.csv

prefix=./data/
sufix=_grounds.csv

for i in {3..20}
do
    ./a.out $prefix"$(($i*50))"$sufix
done