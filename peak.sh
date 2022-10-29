#!/bin/bash
echo "found:"
ls ./datath/*x.txt | xargs grep -v '#' | tail -n 20
echo "min - max:"
ls ./datath/*x.txt | xargs -I {} sh -c "cat {} | tail -1" | grep "#" | cut -d' ' -f2- | sort -n | head -1 #least progressed core 
ls ./datath/*x.txt | xargs -I {} sh -c "cat {} | tail -1" | grep "#" | cut -d' ' -f2- | sort -n | tail -1 #most -||-
