set term png
set grid
set key left top
set title "Matrix multiplication"
set xlabel "N"
set ylabel "time (seconds)"

set output "sgemm_blas.png"
plot 'naive.dat' with linespoints title 'Naive',\
     'strassen.dat' with linespoints t 'Strassen'
