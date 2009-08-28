set title "CPU Utilization"
set xdata time
set timefmt "%H:%M:%S"
set format x "%H:%M"
plot "data" using 1:4 title "%user" with lines, \
	"data" using 1:6 title "%system" with lines, \
	"data" using 1:7 title "%iowait" with lines
pause -1
