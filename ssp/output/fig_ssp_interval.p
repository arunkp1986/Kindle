set terminal postscript eps enhanced color size 3.7,2

red = "#FF0000"; green = "#0C966A"; blue = "#0000FF"; skyblue = "#87CEEB"; orange = "#FFA500"; purple = "#800080"; maroon = "#800000"; black = "#000000"

#set multiplot layout 2,1 margins char 8,1,4,2
set offset -0.4,-0.4,0,0
set lmargin 10
set rmargin 1
set bmargin 4
set tmargin 2
set style data histogram
#set style histogram rowstacked
set style histogram cluster gap 1 
set style fill pattern 9 border -1
set boxwidth 0.9 relative
set ytics nomirror font ", 25"
set xtics nomirror font ", 25"
set yrange [0:]
set xlabel "Benchmarks" font "helvetica, 30" offset 0,-0.5,0
set ylabel "Execution Time\n\n(Normalized)" font "helvetica, 30" offset -2,-0.5,0
set key reverse Left samplen 1 font ",23" at 2.56,6.5,3
set arrow from graph 0,first 1 to graph 1,first 1 nohead dashtype 0 lw 3 lc rgb "#000000" front
set output 'figure_5.eps'
plot "ssp_interval.tsv" using 2:xtic(1) title '1ms' lc rgb blue,\
     "" using 3 title "5ms" lc rgb green,\
     "" using 4 title "10ms" lc rgb maroon

