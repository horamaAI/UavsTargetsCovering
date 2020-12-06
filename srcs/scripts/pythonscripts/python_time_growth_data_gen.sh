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

targets=(set1_1000_1000_map set2_1500_1500_map set3_2000_2000_map set4_2500_2500_map set5_3000_3000_map set6_3500_3500_map)

baselimit=0

# 6 sets of "spreadness"
for j in {1..6}
do

    baselimit=200*j

    # for all 20 instances: from 50 targets to 1000
    for i in {1..20}
    do

        xinf=$(( $baselimit + $(( $(( $i*50 )) + $(( $(( $j*150 )) )) )) ))
        yinf=$(( $baselimit + $(( $(( $i*50 )) + $(( $(( $j*150 )) )) )) ))

        printf "# gen centroids\n" > ../params_custom
        printf "delta="75"\n" >> ../params_custom
        printf "crop=$(($i*10))\n" >> ../params_custom

        withptdevi=0

        if [ $j -lt 4 ] && [ $i -lt 11 ]; then
            withptdevi=$((125+($i*5)+($j*10)))
        else
            withptdevi=$((125+($i*20)+($j*30)))
        fi

        printf "withinpointsdeviation=$withptdevi\n" >> ../params_custom

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
        r=`echo "scale=2; 1+($j/9)+($i/30)" | bc`
        printf "r=$r\n" >> ../params_custom
        #printf "r=%.2f\n" "$((1.0+$((10**3 * $j/6))e-2))" >> ../params_custom
        r2=`printf "%.3f\n" "$(($((10**3 * $i/20))+$((10**3 * $j/6))))e-3"`
        printf "r2=$r2\n" >> ../params_custom
        printf "delta="50"\n" >> ../params_custom
        printf "nrelaxed=$(($i*10))\n" >> ../params_custom
        printf "nscattered="0"\n" >> ../params_custom
        printf "maxpercluster="0"\n" >> ../params_custom
        printf "maxtourperrandom="100"\n" >> ../params_custom

        # cat ../params_custom
        echo $i", "$j", "$xinf", "$yinf
        printf "r=$r\n"
        printf "r2=$r2\n"
        printf "withinpointsdeviation=$withptdevi\n"

        # generate input data from previous parameters
        python3 gen_large_data.py ../params_custom ${targets[$j-1]}

    done

done