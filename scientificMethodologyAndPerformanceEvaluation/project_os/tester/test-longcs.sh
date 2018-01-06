#!/bin/bash
eval "reps=({1..${2}})"
for i in {0..200..5}
do
	echo -n "$i "
	make $1-longcs CSL=$i > /dev/null
	for j in "${reps[@]}"
	do
		./$1-longcs.run | grep throughput;
	done | awk	'BEGIN	{thr = 0; lat = 0; fair; i = 0}
				{thr += $2; lat += $4; fair += $6; i += 1} 
		END 	{thr = thr/i; lat = lat/i; fair = fair/i; printf "throughput: %d\tlatency: %d\tfairness: %f\n", thr, lat, fair; }'
done
