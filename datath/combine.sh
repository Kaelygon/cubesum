#!/bin/bash
cat ./datath/*x.txt >> ./datath/cubes.txt
sort -n ./datath/cubes.txt | grep "^[ 0-9]" >> ./datath/cubesSorted.txt
#rm ./datath/*x.txt
#rm ./datath/cubes.txt