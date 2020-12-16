
# https://stackoverflow.com/questions/23327149/how-can-i-create-a-boxplot-for-each-x-value
# https://github.com/gnuplot/gnuplot/blob/master/demo/energy_circles.dat
# https://stackoverflow.com/questions/15404628/how-can-i-generate-box-and-whisker-plots-with-variable-box-width-in-gnuplot

set datafile separator ","

plot 'data/1000_grounds_coords.csv' lc rgb 'red' notitle