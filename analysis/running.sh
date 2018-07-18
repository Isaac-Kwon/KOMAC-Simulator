#!/bin/bash
for i in 0 1 2 3 4 5 6 7 8 9
do
	for j in 0 1 2 3 4 5 6 7 8 9
	do
		root -q -b -l draw.C\(\"result/sim_17MeV/run_405\",\"run1_${i}_${j}\",-1\)
	done
done
