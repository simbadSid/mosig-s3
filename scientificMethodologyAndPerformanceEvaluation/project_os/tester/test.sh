#!/bin/bash
readonly TEMP=/tmp/test.tmp
eval "cores=({${2}..${3}})"
eval "reps=({1..${4}})"
for i in "${cores[@]}"
do
	echo -n > $TEMP
	echo -n "$i "
	for j in "${reps[@]}"
	do
		./$1.run $i 1 >> $TEMP
	done
	cat $TEMP | grep throughput | awk	'BEGIN	{thr = 0; lat = 0; fair; i = 0}
				{thr += $2; lat += $4; fair += $6; i += 1} 
		END 	{thr = thr/i; lat = lat/i; fair = fair/i; printf "throughput: %d\tlatency: %d\tfairness: %f\n", thr, lat, fair; }'
	cat $TEMP | grep combining | awk	'BEGIN	{comb = 0; i = 0}
				{comb += $3; i+=1} 
		END 	{if (i) comb /= i; else comb = 0; printf "combining degree: %f\n", comb; }'
	cat $TEMP | grep CAS | awk	'BEGIN	{perop = 0; succ = 0}
				{perop += $2; succ += $3; i+=1} 
		END 	{if (i) { perop /= i; succ /= i; } else perop = succ = 0; printf "CAS/op: %f; CAS success rate: %f\n", perop, succ; }'
done
rm $TEMP
