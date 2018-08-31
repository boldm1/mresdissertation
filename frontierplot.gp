set terminal postscript enhanced "Helvetica" 25

set output "efficientfrontier.eps"

set xlabel "Cost"
set ylabel "Hazard"

set datafile separator ','

set offset 1,1,1,1

plot "4btsolfile1.csv" using 1:2:(sprintf("%d", column(0)+1)) with labels point pt 7 offset char 1,1 notitle
