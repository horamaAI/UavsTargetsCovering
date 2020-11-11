
set terminal png

set datafile separator ","
set yrange[-10:35]
set xlabel "n target nodes"
set ylabel "runtime in seconds"

set output 'runtimes_large_gaps.png'

# generated with python script, see find_min_evol_growth.py
#array a = [0.00451, 0.010659, 0.028296, 0.057878, 0.067385, 0.150144, 0.13924, 0.190528, 0.276756, 0.401455, 0.328851,
# 0.554486, 1.412458, 2.03999, 2.612296, 18.406096, 19.748831, 17.259284, 29.647016, 30.984802]

# used in shell (copied) for i in {1..20}; do echo -n \"$(($i*50))\"\ $i,\ ; done
set xtics ("100" 2, "200" 4, "300" 6, "400" 8, "500" 10, "600" 12, "700" 14, "800" 16, "900" 18, "1000" 20)

# called from ./srcs/scripts/
plot for [n=2:4]'../outs/results_runtimes.csv' u n w linespoints notitle