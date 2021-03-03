reset
set ylabel 'time(nsec)'
set title 'Performance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'
set xlabel 'experiment'

plot [][:10000000] 'bench' using 1 with points title 'quick sort with recursion', \
'' using 2 with points title 'quick sort with no recursion'