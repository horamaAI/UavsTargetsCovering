#!/bin/bash

#printf \
#"# n targets, lower bound scp, 1st phase n centroids, 2nd phase n coverages, final n uavs deployed, total redundancy, runtime\n" \
#> ./out/growthruntime.csv

prefix=./data/
sufix=_grounds.csv

x0=0
y0=0

xinf=200
yinf=200

base=0

# 6 sets of "spreadness"
for j in {1..6}
do

    base=200*j

    # for all 20 instances: from 50 targets to 1000
    for i in {1..20}
    do

        xinf=base+(i*50)
        yinf=base+(i*50)

        printf "# gen centroids\n" > ../params_custom
        printf "delta="75"\n" >> ../params_custom
        printf "crop="75"\n" >> ../params_custom
        printf "withinpointsdeviation=$((125+($i*30)))\n" >> ../params_custom
        printf "maxtours="30"\n" >> ../params_custom
        printf "centroidseed="235.699","303.518"\n" >> ../params_custom
        printf "x0,xinf=$x0,$xinf\n" >> ../params_custom
        printf "y0,yinf=$y0,$yinf\n" >> ../params_custom
        printf "\n" >> ../params_custom
        printf "# gen large data\n" >> ../params_custom
        printf "dim="2"\n" >> ../params_custom
        printf "x0,xinf=$x0,$xinf\n" >> ../params_custom
        printf "y0,yinf=$y0,$yinf\n" >> ../params_custom
        printf "ntargets=$(($i*50))\n" >> ../params_custom
        printf "uavsradius="125"\n" >> ../params_custom
        printf "r="1.5"\n" >> ../params_custom
        printf "r2="0.75"\n" >> ../params_custom
        printf "delta="50"\n" >> ../params_custom
        printf "nrelaxed=$(($i*10))\n" >> ../params_custom
        printf "nscattered="0"\n" >> ../params_custom
        printf "maxpercluster="0"\n" >> ../params_custom
        printf "maxtourperrandom="100"\n" >> ../params_custom

        # generate input data from previous parameters
        python3 gen_large_data.py ../params_custom

    done

done