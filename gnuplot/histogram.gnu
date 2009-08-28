set terminal png font arial 10
#set terminal png transparent nocrop enhanced font arial 8
set encoding iso_8859_1
set output "results.png"
set title "Comparacao entre Varnish e Squid"
set ylabel "reqs/s"
set style data histogram
set style histogram cluster gap 2
set style fill solid border -1
set boxwidth 0.9
set xtic rotate by -30
set bmargin 5
set rmargin 12
plot 'results' using 2:xtic(1) title col, \
	'' u 3 ti col, '' u 4 ti col
