
# set terminal png

set datafile separator ","

# to skip first line ? Didn't work :-/
# ? set key autotitle columnhead
# ? unset key

# set yrange[-10:35]
set xlabel "n target nodes"
set ylabel "runtime (in seconds)"

# legends go outside below
set key outside below

# set output 'runtimes_large_gaps.png'

# generated with python script, see find_min_evol_growth.py
# array a = [0.00451, 0.010659, 0.028296, 0.057878, 0.067385, 0.150144, 0.13924, 0.190528, 0.276756, 0.401455, 0.328851,
# 0.554486, 1.412458, 2.03999, 2.612296, 18.406096, 19.748831, 17.259284, 29.647016, 30.984802]

# used in shell (copied) for i in {1..20}; do echo -n \"$(($i*50))\"\ $i,\ ; done
set xtics ("50" 0, "150" 2, "250" 4, "350" 6, "450" 8, "550" 10, "650" 12, "750" 14, "850" 16, "950" 18)

# called from ./srcs/scripts/
# plot for [n=2:4]'../outs/results_runtimes.csv' u n w linespoints notitle

plot 'set_large_gaps_for_cc_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 'set_large_gaps_for_cc_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 'set_large_gaps_for_cc_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 'set_large_gaps_for_cc_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot '2_data_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 '2_data_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 '2_data_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 '2_data_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints
 
plot 'set1_1000_1000_map_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 'set1_1000_1000_map_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 'set1_1000_1000_map_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 'set1_1000_1000_map_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot 'set2_1500_1500_map_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 'set2_1500_1500_map_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 'set2_1500_1500_map_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 'set2_1500_1500_map_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot 'set3_2000_2000_map_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 'set3_2000_2000_map_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 'set3_2000_2000_map_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 'set3_2000_2000_map_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot 'set4_2500_2500_map_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 'set4_2500_2500_map_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 'set4_2500_2500_map_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 'set4_2500_2500_map_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot 'set5_3000_3000_map_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 'set5_3000_3000_map_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 'set5_3000_3000_map_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 'set5_3000_3000_map_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot 'set6_3500_3500_map_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 'set6_3500_3500_map_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 'set6_3500_3500_map_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 'set6_3500_3500_map_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot 'data_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 'data_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 'data_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 'data_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot '1_data_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 '1_data_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 '1_data_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 '1_data_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot '2_data_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 '2_data_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 '2_data_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 '2_data_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints

plot '3_data_results_runtimes.csv' u 1 title 'tot runtime' with linespoints,\
 '3_data_results_runtimes.csv' u 2 title '1st phase, clustering' with linespoints,\
 '3_data_results_runtimes.csv' u 3 title 'MIP solver' with linespoints,\
 '3_data_results_runtimes.csv' u 4 title 'linking one connected component' with linespoints