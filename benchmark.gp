reset
set ylabel 'time(nsec)'
set title 'Performance comparison'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'
set xlabel 'experiment'

plot [][] 'sort.txt' using 1 with line title 'intro sort', \
'' using 2 with line title 'tree sort', \
'' using 3 with line title 'quick sort with no recursion', \
'' using 4 with line title 'quick sort with recursion', \