set terminal postscript eps enhanced color size 3.5,2.5

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
set yrange [1:]
set format y "10^{%L}"
set logscale y 10
set xlabel "Memory Allocation Size" font "helvetica, 30" offset 0,-0.5,0
set ylabel "Execution Time(msec)" font "helvetica, 30" offset -3,-0.5,0
set key left top Right samplen 1 font ",25"
set output 'figure_4_a.eps'
plot "pagetable_case1.tsv" using 2:xtic(1) title 'Persistent' lc rgb blue,\
     "" using 3 title "Rebuild" lc rgb green
