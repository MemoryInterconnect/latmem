#!/bin/bash

STRIDES=(8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304)
LOG=latmem_multi_result.txt

for i in ${STRIDES[@]}; do
	echo "" >> $LOG
	echo "latmemfar $((1024*1024)) $i" >> $LOG
	for j in $(seq 1 3); do
		./latmemfar_noforloop 0x200000000 $((1024*1024)) $i|tee -a $LOG
	done
done

for i in ${STRIDES[@]}; do
	echo "" | tee -a $LOG
	echo "latmem $((1024*1024)) $i" | tee -a $LOG
	for j in $(seq 1 3); do
		./latmem $((1024*1024)) $i|tee -a $LOG
	done
done


